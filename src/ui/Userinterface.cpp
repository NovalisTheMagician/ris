#include "RIS.hpp"
#include "loader/Loader.hpp"
#include "input/Input.hpp"
#include "window/Window.hpp"

#include "ui/Userinterface.hpp"

#include "misc/Timer.hpp"
#include "misc/Logger.hpp"

#include <algorithm>

#include "misc/StringSupport.hpp"
#include "misc/MathHelper.hpp"

#include <memory>

using namespace std::literals;

namespace RIS::UI
{
    void Userinterface::PostInit(Loader::ResourcePack &resourcePack)
    {
        renderer = std::make_unique<Graphics::SpriteRenderer>(resourcePack);

        defaultFont = Loader::Load<Graphics::Font>("fonts/unispace.json", resourcePack);

        Config &config = GetConfig();
        screenWidth = config.GetValue("r_width", 800);
        screenHeight = config.GetValue("r_height", 600);

        uiScale = config.GetValue("ui_scale", 1.0f);

        uiWidth = screenWidth;
        uiHeight = screenHeight;

        uiFramebuffer = Graphics::Framebuffer(uiWidth, uiHeight);

        console.InitLimits(glm::vec2(screenWidth, screenHeight), resourcePack);

        console.BindFunc("fps", Helpers::BoolFunc(showFps, "Show FPS", "Hide FPS"));
        console.BindFunc("frametime", Helpers::BoolFunc(showFrametime, "Show Frametime", "Hide Frametime"));
        console.BindFunc("debugdata", Helpers::BoolFunc(showDebugData, "Show Debugging Information", "Hide Debugging Information"));

        auto &input = GetInput();
        input.RegisterChar([this](uint32_t ch){ return OnChar(ch); });
        input.RegisterMouse([this](float x, float y){ return OnMouseMove(x, y); });
        input.RegisterWheel([this](float x, float y){ return OnMouseWheel(x, y); });
        input.RegisterButtonDown([this](Input::InputKey button){ return OnMouseDown(button); });
        input.RegisterButtonUp([this](Input::InputKey button){ return OnMouseUp(button); });
        input.RegisterKeyDown([this](Input::InputKey key){ return OnKeyDown(key); });
        input.RegisterKeyUp([this](Input::InputKey key){ return OnKeyUp(key); });
        input.RegisterKeyRepeat([this](Input::InputKey key){ return OnKeyRepeat(key); });
    }

    void Userinterface::ReleaseScriptReferences()
    {
        menus.clear();
    }

    void Userinterface::Invalidate()
    {
        
    }

    void Userinterface::RegisterScriptFunctions()
    {
        
    }

    Console &Userinterface::GetConsole()
    {
        return console;
    }

    void Userinterface::Draw()
    {
        renderer->Begin();

        uiFramebuffer.Bind();
        uiFramebuffer.Clear(Graphics::Colors::Transparent, 1.0f);

        if(!activeMenus.empty())
            activeMenus.top().get().Draw(*renderer, {0, 0});

        renderer->SetViewport(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
        defaultFramebuffer.Bind();

        renderer->DrawTexture(uiFramebuffer.ColorTexture(), {0, 0}, {screenWidth, screenHeight});

        console.Draw(*renderer);

        float verticalOffset = 0.0f;
        const float fontSize = debugFontSize * uiScale;
        const float verticalAdvance = defaultFont->GetMaxHeight(fontSize);
        if(showFps)
        {
            renderer->DrawString(std::to_string(fps), *defaultFont, fontSize, glm::vec2(0, verticalOffset));
            verticalOffset += verticalAdvance;
            if(showFrametime)
            {
                renderer->DrawString(std::to_string(1.0f / fps), *defaultFont, fontSize, glm::vec2(0, verticalOffset));
                verticalOffset += verticalAdvance;
            } 
        }

        if(showDebugData)
        {
            for(auto &[name, value] : debugData)
            {
                std::string str = fmt::format("{}: {}", name, value);
                renderer->DrawString(str, *defaultFont, fontSize, glm::vec2(0, verticalOffset));
                verticalOffset += verticalAdvance;
            }
        }

        renderer->End();
    }

    int nFrames = 0;
    float lastTime = 0;

    void Userinterface::Update(const Timer &timer)
    {
        frameTime = timer.Delta();

        nFrames++;
        lastTime += frameTime;
        if(lastTime >= 1.0f)
        {
            fps = nFrames;
            nFrames = 0;
            lastTime -= 1.0f;
        }
        
        if(!activeMenus.empty())
            activeMenus.top().get().Update(timer);
        console.Update(timer);
    }

    int Userinterface::GetWidth() const
    {
        return uiWidth;
    }

    int Userinterface::GetHeight() const
    {
        return uiHeight;
    }

    Panel& Userinterface::CreateMenu(const std::string &menuName)
    {
        Panel p(uiFramebuffer, defaultFont, glm::vec2(uiWidth, uiHeight));
        menus.emplace(menuName, std::move(p));
        return menus.at(menuName);
    }

    void Userinterface::PushMenu(const std::string &menuName)
    {
        if(menus.count(menuName) > 0)
        {
            if(activeMenus.empty())
                GetWindow().SetRelativeMouse(false);
            auto &menu = menus.at(menuName);
            menu.Reset();
            activeMenus.push(std::ref(menu));
        }
    }

    void Userinterface::PopMenu()
    {
        if(!activeMenus.empty())
        {
            activeMenus.pop();

            if(activeMenus.empty())
                GetWindow().SetRelativeMouse(true);
        }
    }

    std::optional<std::reference_wrapper<Panel>> Userinterface::GetMenu(const std::string &menuName)
    {
        if(menus.count(menuName) > 0)
        {
            return std::ref(menus.at(menuName));
        }
        return std::nullopt;
    }

    bool Userinterface::IsMenuOpen() const
    {
        return !activeMenus.empty();
    }

    Graphics::Font::Ptr Userinterface::GetDefaultFont() const
    {
        return defaultFont;
    }

    bool Userinterface::OnChar(uint32_t character)
    {
        if(console.IsOpen())
        {
            console.OnChar(character);
            return true;
        }
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnChar(character);
            return true;
        }
        return false;
    }

    std::unordered_map<std::string, std::string>& Userinterface::GetDebugData()
    {
        return debugData;
    }

    bool Userinterface::OnMouseMove(float x, float y)
    {
        if(!activeMenus.empty())
        {
            float mx = uiWidth * (x / screenWidth);
            float my = uiHeight * (y / screenHeight);

            activeMenus.top().get().OnMouseMove(mx, my);
            return true;
        }
        return false;
    }

    bool Userinterface::OnMouseDown(Input::InputKey button)
    {
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnMouseDown(button);
            return true;
        }
        return false;
    }

    bool Userinterface::OnMouseUp(Input::InputKey button)
    {
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnMouseUp(button);
            return true;
        }
        return false;
    }

    bool Userinterface::OnMouseWheel(float x, float y)
    {
        if(console.IsOpen())
        {
            console.OnMouseWheel(x, y);
            return true;
        }
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnMouseWheel(x, y);
            return true;
        }
        return false;
    }

    bool Userinterface::OnKeyDown(Input::InputKey key)
    {
        if(key == Input::InputKey::F1)
        {
            console.Toggle();
            return true;
        }
        if(console.IsOpen())
        {
            console.OnKeyDown(key);
            return true;
        }
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnKeyDown(key);
            return true;
        }
        return false;
    }

    bool Userinterface::OnKeyUp(Input::InputKey key)
    {
        if(console.IsOpen())
        {
            return true;
        }
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnKeyUp(key);
            return true;
        }
        return false;
    }

    bool Userinterface::OnKeyRepeat(Input::InputKey key)
    {
        if(console.IsOpen())
        {
            console.OnKeyRepeat(key);
            return true;
        }
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnKeyRepeat(key);
            return true;
        }
        return false;
    }
}
