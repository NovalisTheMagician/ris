#include "RIS.hpp"
#include "loader/Loader.hpp"
#include "input/Input.hpp"
#include "script/ScriptEngine.hpp"

#include "ui/Userinterface.hpp"

#include "misc/Timer.hpp"

#include "misc/Logger.hpp"

#include <algorithm>

#include "misc/StringSupport.hpp"
#include "misc/MathHelper.hpp"

#include <memory>

using namespace std::literals;

namespace RIS
{
    namespace UI
    {
        Userinterface::Userinterface()
        {
            rootContainer = std::make_shared<Panel>();
        }

        void Userinterface::PostInit()
        {
            renderer = std::make_unique<Graphics::SpriteRenderer>();

            auto &loader = GetLoader();
            defaultFont = loader.Load<Graphics::Font>("fonts/unispace.json");

            Config &config = GetConfig();
            uiWidth = config.GetValue("r_width", 800);
            uiHeight = config.GetValue("r_height", 600);

            console.InitLimits(glm::vec2(uiWidth, uiHeight));

            fpsLabel = std::make_shared<Label>(defaultFont);
            fpsLabel->SetPosition({0, 0});
            fpsLabel->SetText("0");
            fpsLabel->SetTextColor({1, 1, 1, 1});
            fpsLabel->SetFont(defaultFont, 16);

            console.BindFunc("fps", Helpers::BoolFunc(showFps, "Show FPS", "Hide FPS"));
            console.BindFunc("frametime", Helpers::BoolFunc(showFrametime, "Show Frametime", "Hide Frametime"));

            auto &input = GetInput();
            input.RegisterChar([this](char ch){ return OnChar(ch); });
            input.RegisterMouse([this](float x, float y){ return OnMouseMove(x, y); });
            input.RegisterButtonDown([this](Input::InputKey button){ return OnMouseDown(button); });
            input.RegisterButtonUp([this](Input::InputKey button){ return OnMouseUp(button); });
            input.RegisterKeyDown([this](Input::InputKey key){ return OnKeyDown(key); });
        }

        void Userinterface::Invalidate()
        {
            auto &scriptEngine = GetScriptEngine();
            scriptEngine.CallFunction<void>("InitUserinterfaces");
        }

        void Userinterface::RegisterScriptFunctions()
        {
            auto &scriptEngine = GetScriptEngine();
            auto &loader = GetLoader();

            scriptEngine.Register("OpenConsole", [this](){ console.Open(); });
            scriptEngine.Register("CloseConsole", [this](){ console.Close(); });
            scriptEngine.Register("ToggleConsole", [this](){ console.Toggle(); });

            scriptEngine.Register("UI_GetWidth", [this](){ return static_cast<float>(uiWidth); });
            scriptEngine.Register("UI_GetHeight", [this](){ return static_cast<float>(uiHeight); });

            scriptEngine.Register("UI_CreateImage", [this](const char *name)
            {
                ImagePtr image = std::make_shared<Image>();
                image->SetName(name);
                components.push_back(image);
                return image.get();
            });

            scriptEngine.Register("UI_ImageSetPosition", [this](void *image, float x, float y)
            {
                Image *i = static_cast<Image*>(image);
                i->SetPosition({x, y});
            });

            scriptEngine.Register("UI_ImageSetSize", [this](void *image, float w, float h)
            {
                Image *i = static_cast<Image*>(image);
                i->SetSize({w, h});
            });

            scriptEngine.Register("UI_ImageSetImage", [this, &loader](void *image, const char *textureName)
            {
                Image *i = static_cast<Image*>(image);
                auto texture = loader.Load<Graphics::Texture>(textureName);
                i->SetImage(texture);
            });

            scriptEngine.Register("UI_CreateButton", [this](const char *name)
            {
                ButtonPtr button = std::make_shared<Button>(defaultFont);
                button->SetName(name);
                components.push_back(button);
                return button.get();
            });

            scriptEngine.Register("UI_ButtonSetPosition", [this](void *button, float x, float y)
            {
                Button *b = static_cast<Button*>(button);
                b->SetPosition({x, y});
            });

            scriptEngine.Register("UI_ButtonSetSize", [this](void *button, float w, float h)
            {
                Button *b = static_cast<Button*>(button);
                b->SetSize({w, h});
            });

            scriptEngine.Register("UI_ButtonSetText", [this](void *button, const char *text)
            {
                Button *b = static_cast<Button*>(button);
                b->SetText(text);
            });

            scriptEngine.Register("UI_ButtonSetFont", [this, &loader](void *button, const char *fontName)
            {
                Button *b = static_cast<Button*>(button);
                auto font = loader.Load<Graphics::Font>(fontName);
                if(font)
                {
                    b->SetFont(font);
                }
            });

            scriptEngine.Register("UI_ButtonSetFontSize", [this](void *button, float fontSize)
            {
                Button *b = static_cast<Button*>(button);
                b->SetFontSize(fontSize);
            });

            scriptEngine.Register("UI_ButtonSetCallback", [this](void *button, std::function<void()> btnCallback)
            {
                Button *b = static_cast<Button*>(button);
                b->SetCallback(btnCallback);
            });

            scriptEngine.Register("UI_CreatePanel", [this](const char *name)
            {
                PanelPtr panel = std::make_shared<Panel>();
                panel->SetName(name);
                panel->SetPosition({0, 0});
                panel->SetSize({static_cast<float>(uiWidth), static_cast<float>(uiHeight)});
                components.push_back(panel);
                return panel.get();
            });

            scriptEngine.Register("UI_PanelSetPosition", [this](void *panel, float x, float y)
            {
                Panel *p = static_cast<Panel*>(panel);
                p->SetPosition({x, y});
            });

            scriptEngine.Register("UI_PanelSetSize", [this](void *panel, float w, float h)
            {
                Panel *p = static_cast<Panel*>(panel);
                p->SetSize({w, h});
            });

            scriptEngine.Register("UI_PanelAdd", [this](void *panel, void *component)
            {
                Panel *p = static_cast<Panel*>(panel);
                auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                if(comp != std::end(components))
                    p->Add(*comp);
            });

            scriptEngine.Register("UI_PanelRemove", [this](void *panel, void *component)
            {
                Panel *p = static_cast<Panel*>(panel);
                auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                if(comp != std::end(components))
                    p->Remove(*comp);
            });

            scriptEngine.Register("UI_PanelRemoveAll", [this](void *panel)
            {
                Panel *p = static_cast<Panel*>(panel);
                p->RemoveAll();
            });

            scriptEngine.Register("UI_RegisterMenu", [this](const char *menuName, void *component)
            {
                auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                if(comp != std::end(components))
                    menus.insert_or_assign(menuName, *comp);
            });

            scriptEngine.Register("UI_SetActiveMenu", [this](const char *menuName)
            {
                if(menus.count(menuName) > 0)
                {
                    auto& comp = menus.at(menuName);
                    activeMenu = comp;
                }
                else
                {
                    activeMenu = nullptr;
                }
            });
        }

        Console &Userinterface::GetConsole()
        {
            return console;
        }

        void Userinterface::Draw()
        {
            renderer->Begin(static_cast<float>(uiWidth), static_cast<float>(uiHeight));
            if(showFps)
                fpsLabel->Draw(*renderer, glm::vec2());
            if(activeMenu)
                activeMenu->Draw(*renderer, glm::vec2());
            console.Draw(*renderer);
            renderer->End();
        }

        void Userinterface::Update(const Timer &timer)
        {
            frameTime = timer.Delta();
            if(showFrametime)
                fpsLabel->SetText(std::to_string(frameTime));
            else
                fpsLabel->SetText(std::to_string(static_cast<int>(1 / frameTime)));
            if(activeMenu)
                activeMenu->Update();
            console.Update(timer);
        }

        bool Userinterface::OnChar(char character)
        {
            if(console.IsOpen())
            {
                console.OnChar(character);
                return true;
            }
            if(activeMenu)
            {
                activeMenu->OnChar(character);
                return true;
            }
            return false;
        }

        bool Userinterface::OnMouseMove(float x, float y)
        {
            if(activeMenu)
            {
                activeMenu->OnMouseMove(x, y);
                return true;
            }
            return false;
        }

        bool Userinterface::OnMouseDown(Input::InputKey button)
        {
            if(activeMenu)
            {
                activeMenu->OnMouseDown(button);
                return true;
            }
            return false;
        }

        bool Userinterface::OnMouseUp(Input::InputKey button)
        {
            if(activeMenu)
            {
                activeMenu->OnMouseUp(button);
                return true;
            }
            return false;
        }

        bool Userinterface::OnMouseWheel(float x, float y)
        {
            if(activeMenu)
            {
                activeMenu->OnMouseWheel(x, y);
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
            if(activeMenu)
            {
                activeMenu->OnKeyDown(key);
                return true;
            }
            return false;
        }

        bool Userinterface::OnKeyUp(Input::InputKey key)
        {
            if(activeMenu)
            {
                activeMenu->OnKeyUp(key);
                return true;
            }
            return false;
        }
    }
}
