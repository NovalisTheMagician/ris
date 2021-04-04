#include "RIS.hpp"
#include "loader/Loader.hpp"
#include "input/Input.hpp"

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
    Userinterface::Userinterface()
        : fb(200, 200)
    {
        
    }

    void Userinterface::PostInit(Loader::ResourcePack &resourcePack)
    {
        renderer = std::make_unique<Graphics::SpriteRenderer>(resourcePack);

        defaultFont = Loader::Load<Graphics::Font>("fonts/unispace.json", resourcePack);

        Config &config = GetConfig();
        uiWidth = config.GetValue("r_width", 800);
        uiHeight = config.GetValue("r_height", 600);

        console.InitLimits(glm::vec2(uiWidth, uiHeight), resourcePack);

        /*
        fpsLabel = Label::Create(defaultFont);
        fpsLabel->SetPosition({0, 0});
        fpsLabel->SetText("0");
        fpsLabel->SetTextColor({1, 1, 1, 1});
        fpsLabel->SetFont(defaultFont);
        fpsLabel->SetFontSize(16);
        */

        //console.BindFunc("fps", Helpers::BoolFunc(showFps, "Show FPS", "Hide FPS"));
        //console.BindFunc("frametime", Helpers::BoolFunc(showFrametime, "Show Frametime", "Hide Frametime"));

        auto &input = GetInput();
        input.RegisterChar([this](uint32_t ch){ return OnChar(ch); });
        input.RegisterMouse([this](float x, float y){ return OnMouseMove(x, y); });
        input.RegisterWheel([this](float x, float y){ return OnMouseWheel(x, y); });
        input.RegisterButtonDown([this](Input::InputKey button){ return OnMouseDown(button); });
        input.RegisterButtonUp([this](Input::InputKey button){ return OnMouseUp(button); });
        input.RegisterKeyDown([this](Input::InputKey key){ return OnKeyDown(key); });
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
        fb.Bind();
        fb.Clear({0.35f, 0.75f, 0, 1.0f}, 1.0f);
        defaultFramebuffer.Bind();

        renderer->Begin(static_cast<float>(uiWidth), static_cast<float>(uiHeight));
        /*
        if(showFps)
            fpsLabel->Draw(*renderer, glm::vec2());
        */
        if(!activeMenus.empty())
            activeMenus.top().get().Draw(*renderer);
        console.Draw(*renderer);

        renderer->DrawTexture(fb.ColorTexture());

        renderer->End();
    }

    int nFrames = 0;
    float lastTime = 0;

    void Userinterface::Update(const Timer &timer)
    {
        /*
        frameTime = timer.Delta();

        nFrames++;
        lastTime += frameTime;
        if(lastTime >= 1.0f)
        {
            float fps = static_cast<float>(nFrames);
            nFrames = 0;
            lastTime -= 1.0f;

            if(!showFrametime)
                fpsLabel->SetText(std::to_string(static_cast<int>(fps)));
            else
                fpsLabel->SetText(std::to_string(1 / fps));
        }
        */
        
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
        Panel p(defaultFramebuffer, defaultFont);
        menus.emplace(menuName, std::move(p));
        return menus.at(menuName);
    }

    void Userinterface::PushMenu(const std::string &menuName)
    {
        if(menus.count(menuName) > 0)
        {
            auto &menu = menus.at(menuName);
            activeMenus.push(std::ref(menu));
        }
    }

    void Userinterface::PopMenu()
    {
        if(!activeMenus.empty())
        {
            activeMenus.pop();
        }
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

    bool Userinterface::OnMouseMove(float x, float y)
    {
        if(!activeMenus.empty())
        {
            activeMenus.top().get().OnMouseMove(x, y);
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
