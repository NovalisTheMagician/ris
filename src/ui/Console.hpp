#pragma once

#include <glm/glm.hpp>
#include <string>
#include <functional>
#include <vector>

#include "input/KeyDefs.hpp"

#include "graphics/Renderer.hpp"

#include "misc/Timer.hpp"

namespace RIS
{
    namespace UI
    {
        using ConsoleFunc = std::function<std::string(std::vector<std::string>)>;

        namespace Helpers
        {
            inline std::function<std::string(std::vector<std::string>)> BoolFunc(bool &value, const std::string &onMsg, const std::string &offMsg)
            {
                return [&, onMsg, offMsg](auto& params)
                {
                    using namespace std::literals;
                    if(params.size() == 0)
                    {
                        return std::to_string(value);
                    }
                    else
                    {
                        try
                        {
                            value = std::stoi(params.at(0));
                            return value ? onMsg : offMsg;
                        }
                        catch(const std::exception& e)
                        {
                            return "Invalid Value"s;
                        }
                    }
                };
            }
        }

        class Console
        {
        public:
            void InitLimits(const glm::vec2 &viewSize);

            void Open();
            void Close();
            void Toggle();

            void Print(const std::string &msg);

            void BindFunc(const std::string &name, ConsoleFunc func);

            bool IsOpen();

            void Update(const Timer &timer);
            void Draw();

            bool ProcessLine(const std::string &lineToProcess);

            void OnChar(char c);
            void OnKeyDown(Input::InputKey key);

        private:
            std::string SetParam(std::vector<std::string> params);

        private:
            bool isOpen = false;
            bool isMoving = false;

            glm::vec2 viewSize;
            float currentY = 0;
            float maxY;
            float offsetY = 7.5f;

            int consoleFont;
            float consoleFontSize = 22.0f;
            glm::vec4 backgroundColor = glm::vec4(0, 0, 0, 0.95f);
            glm::vec4 fontColor = glm::vec4(0.7f, 0.7f, 0.7f, 1);

            int maxLines;
            float maxLineHeight;
            std::vector<std::string> lines;
            std::string inputLine;

            int historyIndex;
            std::vector<std::string> inputHistory;

            float openSpeed = 1000;

            std::unordered_map<std::string, ConsoleFunc> funcVars;

        };
    }
}
