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

        void Userinterface::ReleaseScriptReferences()
        {
            components.clear();
            menus.clear();
            activeMenu = nullptr;
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

            scriptEngine.Register("openConsole", [this](){ console.Open(); });
            scriptEngine.Register("closeConsole", [this](){ console.Close(); });
            scriptEngine.Register("toggleConsole", [this](){ console.Toggle(); });

            scriptEngine.Register("getUIWidth", [this](){ return static_cast<float>(uiWidth); });
            scriptEngine.Register("getUIHeight", [this](){ return static_cast<float>(uiHeight); });

            scriptEngine.RegisterKlass<Component>("Component");

            scriptEngine.RegisterKlass<Image, Component>("Image")
                .RegisterFunc("setPosition", [](Image *self, float x, float y){ self->SetPosition({x, y}); })
                .RegisterFunc("setSize", [](Image *self, float w, float h){ self->SetSize({w, h}); })
                .RegisterFunc("setImage", [&loader](Image *self, const std::string &image)
                { 
                    auto texture = loader.Load<Graphics::Texture>(image);
                    self->SetImage(texture);
                });

            scriptEngine.Register("createImage", [this](const std::string &name)
            {
                ImagePtr image = std::make_shared<Image>();
                image->SetName(name);
                components.push_back(image);
                return image.get();
            });

            scriptEngine.RegisterKlass<Button, Component>("Button")
                .RegisterFunc("setPosition", [](Button *self, float x, float y){ self->SetPosition({x, y}); })
                .RegisterFunc("setSize", [](Button *self, float w, float h){ self->SetSize({w, h}); })
                .RegisterFunc("setText", &Button::SetText)
                .RegisterFunc("setFont", [&loader](Button *self, const std::string &fontName)
                {
                    auto font = loader.Load<Graphics::Font>(fontName);
                    if(font)
                    {
                        self->SetFont(font);
                    }
                })
                .RegisterFunc("setFontSize", &Button::SetFontSize)
                .RegisterFunc("setCallback", &Button::SetCallback);

            scriptEngine.Register("createButton", [this](const std::string &name)
            {
                ButtonPtr button = std::make_shared<Button>(defaultFont);
                button->SetName(name);
                components.push_back(button);
                return button.get();
            });

            scriptEngine.RegisterKlass<Panel, Component>("Panel")
                .RegisterFunc("setPosition", [](Panel *self, float x, float y){ self->SetPosition({x, y}); })
                .RegisterFunc("setSize", [](Panel *self, float w, float h){ self->SetSize({w, h}); })
                .RegisterFunc("add", [this](Panel *self, Component *component)
                {
                    auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                    if(comp != std::end(components))
                        self->Add(*comp);
                })
                .RegisterFunc("remove", [this](Panel *self, Component *component)
                {
                    auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                    if(comp != std::end(components))
                        self->Remove(*comp);
                })
                .RegisterFunc("removeAll", &Panel::RemoveAll);

            scriptEngine.Register("createPanel", [this](const std::string &name)
            {
                PanelPtr panel = std::make_shared<Panel>();
                panel->SetName(name);
                panel->SetPosition({0, 0});
                panel->SetSize({static_cast<float>(uiWidth), static_cast<float>(uiHeight)});
                components.push_back(panel);
                return panel.get();
            });

            scriptEngine.Register("registerMenu", [this](const std::string &menuName, Component *component)
            {
                auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                if(comp != std::end(components))
                    menus.insert_or_assign(menuName, *comp);
            });

            scriptEngine.Register("setActiveMenu", [this](const std::string &menuName)
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
