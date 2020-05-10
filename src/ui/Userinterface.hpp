#pragma once

#include "graphics/TextRenderer.hpp"
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

            void LoadLayout(const std::string &layout);

            Console& GetConsole();

            void Draw();
            void Update(const Timer &timer);

        private:
            void OnChar(char character);
            void OnMouseMove(float x, float y);
            void OnMouseDown(Input::InputButton button);
            void OnMouseUp(Input::InputButton button);
            void OnKeyDown(Input::InputKey key);

        private:
            Console console;
            std::unique_ptr<Graphics::TextRenderer> textRenderer;

            std::shared_ptr<Graphics::Font> defaultFont;

            int uiWidth, uiHeight;
            ContainerPtr rootContainer;

            LabelPtr fpsLabel;
            bool showFps = false;
            bool showFrametime = false;
            float frameTime;

            int currentId = 0;

            std::unordered_map<std::string, int> namedLayouts;
            std::unordered_map<int, ContainerPtr> layouts;
        };
    }
}