#pragma once

#include "common/IUserinterface.hpp"
#include "common/IInput.hpp"

#include "common/SystemLocator.hpp"

namespace RIS
{
    class Console : public IConsole
    {
    public:
        Console(const SystemLocator &systems);
        ~Console();

        void InitLimits(const glm::vec2 &viewSize);

        void Open() override;
        void Close() override;
        void Toggle() override;

        void Print(const std::string &msg) override;

        void BindFunc(const std::string &name, ConsoleFunc func) override;

        bool IsOpen();

        void Update(const Timer &timer);
        void Draw(I2DRenderer &renderer);

        bool ProcessLine(const std::string &lineToProcess);

        void OnChar(char c);
        void OnKeyDown(InputKey key);

    private:
        std::string SetParam(std::vector<std::string> params);

    private:
        const SystemLocator &systems;

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
