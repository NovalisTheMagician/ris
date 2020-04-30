#include "Console.hpp"

#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>

#include "common/StringSupport.hpp"
#include "common/MathHelper.hpp"

namespace RIS
{
    Console::Console(const SystemLocator &systems)
        : systems(systems)
    {
    }

    Console::~Console()
    {
    }

    void Console::InitLimits(const glm::vec2 &viewSize)
    {
        auto &r2d = systems.GetRenderer().Get2DRenderer();

        this->viewSize = viewSize;
        maxY = viewSize.y * 0.5f;
        currentY = viewSize.y;

        consoleFont = r2d.LoadFont("fsex302");
        maxLineHeight = r2d.MaxHeightFont(consoleFont, consoleFontSize);
        maxLines = 512;

        IWindow& wnd = systems.GetWindow();

        BindFunc("con", [this](std::vector<std::string> params){ return SetParam(params); });
        BindFunc("exit", [&wnd](std::vector<std::string> params){ wnd.Exit(0); return ""; });
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
        while(lines.size() >= maxLines - 2)
            lines.pop_back();
        
        lines.insert(lines.begin(), msg);
    }

    void Console::BindFunc(const std::string &name, ConsoleFunc func)
    {
        funcVars.insert_or_assign(lowerCase(name), func);
    }

    bool Console::IsOpen()
    {
        return isOpen;
    }

    void Console::Update(const Timer &timer)
    {
        if(isMoving)
        {
            float dir = -1;
            if(!isOpen)
            {
                dir = -dir;
            }

            currentY += dir * openSpeed * timer.Delta();

            if(isOpen && currentY <= maxY)
            {
                isMoving = false;
                currentY = maxY;
            }
            else if(!isOpen && currentY >= viewSize.y)
            {
                isMoving = false;
                currentY = viewSize.y;
            }
        }
    }

    void Console::Draw(I2DRenderer &renderer)
    {
        if(isOpen || isMoving)
        {
            renderer.SetTexture(1, 0);
            renderer.DrawQuad({0, currentY}, {viewSize.x, maxY}, backgroundColor);

            auto it = lines.begin();
            for(int i = 1; it != lines.end(); ++it, ++i)
            {
                const std::string &msg = *it;
                glm::vec2 pos = {0, (currentY + i * maxLineHeight) + offsetY};
                renderer.DrawText(msg, consoleFont, pos, consoleFontSize, fontColor);
            }
            renderer.DrawText(">" + inputLine + "_", consoleFont, {0, currentY + offsetY}, consoleFontSize, fontColor);
        }
    }

    void Console::OnChar(char c)
    {
        if(isOpen)
        {
            inputLine.insert(inputLine.end(), c);
        }
    }

    void Console::OnKeyDown(InputKey key)
    {
        if(isOpen)
        {
            if(key == InputKey::ENTER || key == InputKey::KP_ENTER)
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
            else if(key == InputKey::BACKSPACE)
            {
                if(inputLine.size() > 0)
                    inputLine.erase(inputLine.end() - 1);
            }
            else if(key == InputKey::UP)
            {
                if(!inputHistory.empty())
                {
                    inputLine = inputHistory[historyIndex++];
                    historyIndex = Clamp(0, static_cast<int>(inputHistory.size())-1, historyIndex);
                }
            }
            else if(key == InputKey::DOWN)
            {
                if(!inputHistory.empty())
                {
                    inputLine = inputHistory[historyIndex--];
                    historyIndex = Clamp(0, static_cast<int>(inputHistory.size())-1, historyIndex);
                }
            }
        }
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
            
            backgroundColor.r = std::stof(params[1]);
            backgroundColor.g = std::stof(params[2]);
            backgroundColor.b = std::stof(params[3]);
            backgroundColor.a = std::stof(params[4]);
        }
        else if(params[0] == "forground")
        {
            if(params.size() < 5)
                return "not enough color data";
            
            fontColor.r = std::stof(params[1]);
            fontColor.g = std::stof(params[2]);
            fontColor.b = std::stof(params[3]);
            fontColor.a = std::stof(params[4]);
        }
        else if(params[0] == "font")
        {
            if(params.size() < 2)
                return "no font specified";
            
            consoleFont = systems.GetRenderer().Get2DRenderer().LoadFont(params[1]);
            maxLineHeight = systems.GetRenderer().Get2DRenderer().MaxHeightFont(consoleFont, consoleFontSize);
        }
        else if(params[0] == "fontsize")
        {
            if(params.size() < 2)
                return "no size specified";
            
            consoleFontSize = std::stof(params[1]);
            maxLineHeight = systems.GetRenderer().Get2DRenderer().MaxHeightFont(consoleFont, consoleFontSize);
        }
        else
        {
            return "unkown param " + params[0];
        }

        return "";
    }
}
