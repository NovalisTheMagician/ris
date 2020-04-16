#pragma once

#include "common/IUserinterface.hpp"
#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include "Console.hpp"

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

namespace RIS
{
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
        void OnMouseDown(InputButton button);
        void OnMouseUp(InputButton button);
        void OnKeyDown(InputKey key);

    private:
        const SystemLocator &systems;
        Config &config;

        Console console;

        int uiWidth, uiHeight;
        ContainerPtr rootContainer;

        LabelPtr fpsLabel;
        bool showFps = false;
        bool showFrametime = false;
        float frameTime;

        int currentId = 0;

        std::unordered_map<std::string, int> namedLayouts;
        std::unordered_map<int, ContainerPtr> layouts;

        ResourceId uiFramebufferId = -1;
    };
}