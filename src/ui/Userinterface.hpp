#pragma once

#include "graphics/SpriteRenderer.hpp"
#include "graphics/Font.hpp"

#include "ui/Console.hpp"

#include "input/KeyDefs.hpp"

#include <glm/glm.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include <memory>

#include "ui/Component.hpp"
#include "ui/Container.hpp"
#include "ui/Panel.hpp"
#include "ui/Label.hpp"
#include "ui/Image.hpp"
#include "ui/Button.hpp"
#include "ui/Inputbox.hpp"

namespace RIS
{
    namespace UI
    {
        class Userinterface
        {
        public:
            Userinterface();
            ~Userinterface() = default;

            void PostInit();
            void RegisterScriptFunctions();

            void Invalidate();

            Console& GetConsole();

            void Draw();
            void Update(const Timer &timer);

        private:
            bool OnChar(char character);
            bool OnMouseMove(float x, float y);
            bool OnMouseDown(Input::InputKey button);
            bool OnMouseUp(Input::InputKey button);
            bool OnMouseWheel(float x, float y);
            bool OnKeyDown(Input::InputKey key);
            bool OnKeyUp(Input::InputKey key);

        private:
            Console console;
            std::unique_ptr<Graphics::SpriteRenderer> renderer;

            std::shared_ptr<Graphics::Font> defaultFont;

            int uiWidth, uiHeight;
            ContainerPtr rootContainer;

            LabelPtr fpsLabel;
            bool showFps = false;
            bool showFrametime = false;
            float frameTime;

            ComponentPtr activeMenu;

            std::unordered_map<std::string, ComponentPtr> menus;
            std::vector<ComponentPtr> components;
        };
    }
}