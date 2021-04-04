#pragma once

#include "graphics/SpriteRenderer.hpp"
#include "graphics/Font.hpp"
#include "graphics/Framebuffer.hpp"

#include "ui/Console.hpp"

#include "input/KeyDefs.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <stack>

#include "ui/Component.hpp"
#include "ui/Panel.hpp"
#include "ui/Label.hpp"
#include "ui/Image.hpp"
#include "ui/Button.hpp"
#include "ui/Inputbox.hpp"

#include "loader/ResourcePack.hpp"

namespace RIS::UI
{
    class Userinterface
    {
    public:
        void PostInit(Loader::ResourcePack &resourcePack);
        void RegisterScriptFunctions();

        void ReleaseScriptReferences();
        void Invalidate();

        Console& GetConsole();

        void Draw();
        void Update(const Timer &timer);

        int GetWidth() const;
        int GetHeight() const;

        Panel& CreateMenu(const std::string &menuName);

        void PushMenu(const std::string &menuName);
        void PopMenu();

        Graphics::Font::Ptr GetDefaultFont() const;

    private:
        bool OnChar(uint32_t character);
        bool OnMouseMove(float x, float y);
        bool OnMouseDown(Input::InputKey button);
        bool OnMouseUp(Input::InputKey button);
        bool OnMouseWheel(float x, float y);
        bool OnKeyDown(Input::InputKey key);
        bool OnKeyUp(Input::InputKey key);
        bool OnKeyRepeat(Input::InputKey key);

    private:
        Console console;
        std::unique_ptr<Graphics::SpriteRenderer> renderer;

        Graphics::Font::Ptr defaultFont;
        Graphics::Framebuffer defaultFramebuffer;

        int uiWidth, uiHeight;

        //Label fpsLabel;
        //bool showFps = false;
        //bool showFrametime = false;
        //float frameTime;

        std::stack<std::reference_wrapper<Panel>> activeMenus;
        std::unordered_map<std::string, Panel> menus;
    };
}