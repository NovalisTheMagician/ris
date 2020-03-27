#pragma once

#include "common/IUserinterface.hpp"
#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <glm/glm.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <variant>

#include "Component.hpp"
#include "Container.hpp"
#include "Panel.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "Button.hpp"
#include "Inputbox.hpp"

#include "UIAction.hpp"

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
        void OnKeyDown(InputKeys key);

    private:
        std::string SetParam(std::vector<std::string> params);

    private:
        const SystemLocator &systems;

        bool isOpen;
        bool isMoving;

        glm::vec2 viewSize;
        float currentY;
        float maxY;
        float offsetY;

        int consoleFont;
        float consoleFontSize;
        glm::vec4 backgroundColor;
        glm::vec4 fontColor;

        int maxLines;
        float maxLineHeight;
        std::vector<std::string> lines;
        std::string inputLine;

        int historyIndex;
        std::vector<std::string> inputHistory;

        float openSpeed;

        std::unordered_map<std::string, ConsoleFunc> funcVars;

    };

    class SimpleUserinterface : public IUserinterface
    {
    public:
        SimpleUserinterface(const SystemLocator &systems, Config &config);
        ~SimpleUserinterface();

        void PostInit() override;

        void LoadLayout(const std::string &layout) override;

        IConsole& GetConsole() override;

        void Draw() override;
        void Update(const Timer &timer) override;

    private:
        void OnChar(char character);
        void OnMouseMove(float x, float y);
        void OnMouseDown(InputButtons button);
        void OnMouseUp(InputButtons button);
        void OnKeyDown(InputKeys key);

        void LoadPanel(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadButton(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadLabel(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadImage(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadTextbox(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);

    private:
        static const int JSON_VERSION;

    private:
        const SystemLocator &systems;
        Config &config;

        Console console;

        int uiWidth, uiHeight;
        ContainerPtr rootContainer;

        std::unordered_map<std::string, ContainerPtr> layouts;

        int uiFramebufferId;

        // need to make some friends
        friend UIPanel;
        friend UIButton;
        friend UIImage;
        friend UILabel;
        friend UIInputBox;

    };
}