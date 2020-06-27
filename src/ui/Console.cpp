#include "RIS.hpp"
#include "loader/Loader.hpp"
#include "window/Window.hpp"

#include "ui/Console.hpp"

#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>

#include "misc/StringSupport.hpp"
#include "misc/MathHelper.hpp"

#include <utf8.h>
#include <fmt/format.h>

#include <cmath>

namespace RIS
{
    namespace UI
    {
        void Console::InitLimits(const glm::vec2 &viewSize)
        {
            auto &loader = GetLoader();

            this->viewSize = viewSize;
            maxY = 0;
            currentY = -(viewSize.y * 0.5f);

            consoleFont = loader.Load<Graphics::Font>("fonts/immortal.json");
            maxLineHeight = consoleFont->GetMaxHeight(consoleFontSize);
            maxLines = 512;

            auto& wnd = GetWindow();
            BindFunc("con", [this](std::vector<std::string> params){ return SetParam(params); });
            BindFunc("exit", [&wnd](std::vector<std::string> params){ wnd.Exit(0); return ""; });
            BindFunc("clear", [this](std::vector<std::string> params){ Clear(); return ""; });
        }

        void Console::Open()
        {
            if(!isOpen)
            {
                isOpen = true;
                isMoving = true;
            }
        }

        void Console::Close()
        {
            if(isOpen)
            {
                isOpen = false;
                isMoving = true;
            }
        }

        void Console::Toggle()
        {
            if(isOpen)
                Close();
            else 
                Open();
        }

        void Console::Print(const std::string &msg)
        {
            auto msgs = split(msg, "\n");
            while(lines.size() >= maxLines - (msgs.size() + 1))
                lines.pop_back();
            
            for(auto& m : msgs)
                lines.insert(lines.begin(), m);
        }

        void Console::BindFunc(const std::string &name, ConsoleFunc func)
        {
            funcVars.insert_or_assign(lowerCase(name), func);
        }

        bool Console::IsOpen()
        {
            return isOpen;
        }

        void Console::Clear()
        {
            lines.clear();
            viewOffset = 0;
        }

        int roundHalf(float f)
        {
            return static_cast<int>(std::floorf(f + 0.5f));
        }

        void Console::Update(const Timer &timer)
        {
            if(roundHalf(timer.Total()) % 2)
            {
                cursor = "_";
            }
            else
            {
                cursor = "";
            }

            if(isMoving)
            {
                float dir = 1;
                if(!isOpen)
                {
                    dir = -dir;
                }

                currentY += dir * openSpeed * timer.Delta();

                if(isOpen && currentY >= maxY)
                {
                    isMoving = false;
                    currentY = maxY;
                }
                else if(!isOpen && currentY <= -(viewSize.y * 0.5f))
                {
                    isMoving = false;
                    currentY = -(viewSize.y * 0.5f);
                }
            }
        }

        glm::vec2 Console::GetPosForLine(int lineNr)
        {
            float maxHeight = viewSize.y * 0.5f;
            return {0, currentY + (maxHeight - maxLineHeight - (lineNr * maxLineHeight) - offsetY)};
        }

        void Console::Draw(Graphics::SpriteRenderer &renderer)
        {
            if(isOpen || isMoving)
            {
                renderer.DrawRect({0, currentY}, {viewSize.x, viewSize.y * 0.5f}, backgroundColor);

                auto it = lines.begin() + viewOffset;
                for(int i = 1; it != lines.end(); ++it, ++i)
                {
                    const std::string &msg = *it;
                    glm::vec2 pos = GetPosForLine(i);
                    renderer.DrawString(msg, *consoleFont.get(), consoleFontSize, pos, fontColor);
                }
                renderer.DrawString(">" + inputLine + cursor, *consoleFont.get(), consoleFontSize, GetPosForLine(0), fontColor);
            }
        }

        void Console::OnChar(uint32_t c)
        {
            if(isOpen)
            {
                //maybe check exception
                utf8::append(c, std::back_inserter(inputLine));
            }
        }

        void Console::OnKeyDown(Input::InputKey key)
        {
            if(isOpen)
            {
                if(key == Input::InputKey::ENTER || key == Input::InputKey::KP_ENTER)
                {
                    if(inputLine.empty())
                        return;

                    inputHistory.insert(inputHistory.begin(), inputLine);
                    historyIndex = 0;

                    Print(inputLine);
                    if(!ProcessLine(inputLine))
                        Print("Unknown command: \"" + inputLine + "\"");
                    inputLine = "";
                }
                else if(key == Input::InputKey::BACKSPACE)
                {
                    if(inputLine.size() > 0)
                    {
                        auto it = inputLine.end();
                        utf8::prior(it, inputLine.begin());
                        inputLine.erase(it, inputLine.end());
                    }
                }
                else if(key == Input::InputKey::UP)
                {
                    if(!inputHistory.empty())
                    {
                        inputLine = inputHistory[historyIndex++];
                        historyIndex = Clamp(0, static_cast<int>(inputHistory.size())-1, historyIndex);
                    }
                }
                else if(key == Input::InputKey::DOWN)
                {
                    if(!inputHistory.empty())
                    {
                        inputLine = inputHistory[historyIndex--];
                        historyIndex = Clamp(0, static_cast<int>(inputHistory.size())-1, historyIndex);
                    }
                }
                else if(key == Input::InputKey::PAGE_UP)
                {
                    viewOffset++;
                    if(viewOffset > lines.size() - 1)
                        viewOffset = lines.size() - 1;
                }
                else if(key == Input::InputKey::PAGE_DOWN)
                {
                    viewOffset--;
                    if(viewOffset < 0)
                        viewOffset = 0;
                }
                else if(key == Input::InputKey::HOME)
                {
                    viewOffset = lines.size() - 1;
                }
                else if(key == Input::InputKey::END)
                {
                    viewOffset = 0;
                }
            }
        }

        void Console::OnMouseWheel(float x, float y)
        {
            viewOffset += 1 * Signum(y);
            int max = static_cast<int>(lines.size()) - 1;
            viewOffset = Clamp(0, max, viewOffset);
        }

        bool Console::ProcessLine(const std::string &lineToProcess)
        {
            std::istringstream sstream(lineToProcess);
            std::vector<std::string> tokens{std::istream_iterator<std::string>{sstream},
                                std::istream_iterator<std::string>{}};

            std::string keyword = lowerCase(tokens[0]);
            tokens.erase(tokens.begin());
            
            if(funcVars.count(keyword) > 0)
            {
                auto func = funcVars.at(keyword);
                auto &msg = func(tokens);
                if(!msg.empty())
                    Print(msg);
                return true;
            }

            return false;
        }

        std::string Console::SetParam(std::vector<std::string> params)
        {
            if(params.size() < 1)
                return "no param specified!";

            if(params[0] == "background")
            {
                if(params.size() < 5)
                    return "not enough color data";
                
                try 
                {
                    backgroundColor.r = std::stof(params[1]);
                    backgroundColor.g = std::stof(params[2]);
                    backgroundColor.b = std::stof(params[3]);
                    backgroundColor.a = std::stof(params[4]);
                }
                catch(const std::invalid_argument &e)
                {
                    Print(e.what());
                }
            }
            else if(params[0] == "forground")
            {
                if(params.size() < 5)
                    return "not enough color data";
                
                try 
                {
                    fontColor.r = std::stof(params[1]);
                    fontColor.g = std::stof(params[2]);
                    fontColor.b = std::stof(params[3]);
                    fontColor.a = std::stof(params[4]);
                }
                catch(const std::invalid_argument &e)
                {
                    Print(e.what());
                }
            }
            else if(params[0] == "font")
            {
                if(params.size() < 2)
                    return "no font specified";
                
                auto &loader = GetLoader();

                try
                {
                    consoleFont = loader.Load<Graphics::Font>(params[1]);
                    maxLineHeight = consoleFont->GetMaxHeight(consoleFontSize);
                }
                catch(const RISException& e)
                {
                    Print(e.what());
                }
            }
            else if(params[0] == "fontsize")
            {
                if(params.size() < 2)
                    return "no size specified";
                
                try
                {
                    consoleFontSize = std::stof(params[1]);
                    maxLineHeight = consoleFont->GetMaxHeight(consoleFontSize);
                }
                catch(const std::invalid_argument &e)
                {
                    Print(e.what());
                }
            }
            else
            {
                return "unkown param " + params[0];
            }

            return "";
        }
    }
}
