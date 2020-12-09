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
            rootContainer = Panel::Create();
        }

        void Userinterface::PostInit(Loader::ResourcePack &resourcePack)
        {
            renderer = std::make_unique<Graphics::SpriteRenderer>(resourcePack);

            defaultFont = Loader::Load<Graphics::Font>("fonts/unispace.json", resourcePack);

            Config &config = GetConfig();
            uiWidth = config.GetValue("r_width", 800);
            uiHeight = config.GetValue("r_height", 600);

            console.InitLimits(glm::vec2(uiWidth, uiHeight), resourcePack);

            fpsLabel = Label::Create(defaultFont);
            fpsLabel->SetPosition({0, 0});
            fpsLabel->SetText("0");
            fpsLabel->SetTextColor({1, 1, 1, 1});
            fpsLabel->SetFont(defaultFont);
            fpsLabel->SetFontSize(16);

            console.BindFunc("fps", Helpers::BoolFunc(showFps, "Show FPS", "Hide FPS"));
            console.BindFunc("frametime", Helpers::BoolFunc(showFrametime, "Show Frametime", "Hide Frametime"));

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
            components.clear();
            menus.clear();
            activeMenu = nullptr;
        }

        void Userinterface::Invalidate()
        {
            auto &scriptEngine = GetScriptEngine();
            scriptEngine.Call<void>("InitUserinterfaces");
        }

        void Userinterface::RegisterScriptFunctions()
        {
            auto &scriptEngine = GetScriptEngine();

            /*
            scriptEngine.Func("print", [this](const std::string &msg){ console.Print(msg); });
            scriptEngine.Namespace("Console")
                .Func("openConsole", [this](){ console.Open(); })
                .Func("closeConsole", [this](){ console.Close(); })
                .Func("toggleConsole", [this](){ console.Toggle(); });

            scriptEngine.Namespace("UI")
                .Func("getUIWidth", [this](){ return static_cast<float>(uiWidth); })
                .Func("getUIHeight", [this](){ return static_cast<float>(uiHeight); });

            scriptEngine.Class<Component>("Component")
                .Func("setPosition", [](Component *self, float x, float y){ self->SetPosition({x, y}); })
                .Func("setSize", [](Component *self, float w, float h){ self->SetSize({w, h}); });

            scriptEngine.Class<Image, Component>("Image")
                .Func("setImage", [&loader](Image *self, const std::string &image)
                { 
                    auto texture = loader.Load<Graphics::Texture>(image);
                    self->SetImage(texture);
                });

            scriptEngine.Namespace("UI")
                .Func("Image", [this](const std::string &name)
                {
                    Image::Ptr image = Image::Create();
                    image->SetName(name);
                    components.push_back(image);
                    return image.get();
                });

            scriptEngine.Class<Button, Component>("Button")
                .Func("setText", &Button::SetText)
                .Func("setFont", [&loader](Button *self, const std::string &fontName)
                {
                    auto font = loader.Load<Graphics::Font, false>(fontName);
                    if(font)
                        self->SetFont(font);
                })
                .Func("setFontSize", &Button::SetFontSize)
                .Func("setCallback", &Button::SetCallback)
                .Func("setActive", &Button::SetActive);

            scriptEngine.Namespace("UI")
                .Func("Button", [this](const std::string &name)
                {
                    Button::Ptr button = Button::Create(defaultFont);
                    button->SetName(name);
                    components.push_back(button);
                    return button.get();
                });

            auto panelAdd = [this](Panel *self, Component *component)
            {
                auto comp = std::find_if(std::begin(components), std::end(components), 
                [&component](const Component::Ptr &elem) { return component == elem.get(); });
                if(comp != std::end(components))
                    self->Add(*comp);
            };

            auto panelRemove = [this](Panel *self, Component *component)
            {
                auto comp = std::find_if(std::begin(components), std::end(components), 
                [&component](const Component::Ptr &elem) { return component == elem.get(); });
                if(comp != std::end(components))
                    self->Remove(*comp);
            };

            auto panelAddButton = [panelAdd](Panel *self, Button *component){ panelAdd(self, component); };
            auto panelAddLabel = [panelAdd](Panel *self, Label *component){ panelAdd(self, component); };
            auto panelAddImage = [panelAdd](Panel *self, Image *component){ panelAdd(self, component); };
            auto panelAddTextBox = [panelAdd](Panel *self, InputBox *component){ panelAdd(self, component); };

            auto panelRemoveButton = [panelRemove](Panel *self, Button *component){ panelRemove(self, component); };
            auto panelRemoveLabel = [panelRemove](Panel *self, Label *component){ panelRemove(self, component); };
            auto panelRemoveImage = [panelRemove](Panel *self, Image *component){ panelRemove(self, component); };
            auto panelRemoveTextBox = [panelRemove](Panel *self, InputBox *component){ panelRemove(self, component); };

            scriptEngine.Class<Panel, Component>("Panel")
                .FuncOverload("add", panelAddButton, panelAddLabel, panelAddImage, panelAddTextBox)
                .FuncOverload("remove", panelRemoveButton, panelRemoveLabel, panelRemoveImage, panelRemoveTextBox)
                .Func("removeAll", &Panel::RemoveAll);

            scriptEngine.Namespace("UI")
                .Func("Panel", [this](const std::string &name)
                {
                    Panel::Ptr panel = Panel::Create();
                    panel->SetName(name);
                    panel->SetPosition({0, 0});
                    panel->SetSize({static_cast<float>(uiWidth), static_cast<float>(uiHeight)});
                    components.push_back(panel);
                    return panel.get();
                });

            scriptEngine.Class<Label, Component>("Label")
                .Func("setFont", [&loader](Label *self, const std::string &fontName)
                {
                    auto font = loader.Load<Graphics::Font, false>(fontName);
                    if(font)
                        self->SetFont(font);
                })
                .Func("setFontSize", &Label::SetFontSize)
                .Func("setText", &Label::SetText)
                .Func("setTextColor", [](Label *self, float r, float g, float b, float a)
                {
                    self->SetTextColor({ r, g, b, a });
                });

            scriptEngine.Namespace("UI")
                .Func("Label", [this](const std::string &name)
                {
                    Label::Ptr label = Label::Create(defaultFont);
                    label->SetName(name);
                    components.push_back(label);
                    return label.get();
                });

            scriptEngine.Class<InputBox, Component>("Textbox")
                .Func("setFont", [&loader](InputBox *self, const std::string &fontName)
                {
                    auto font = loader.Load<Graphics::Font, false>(fontName);
                    if(font)
                        self->SetFont(font);
                })
                .Func("setFontSize", &InputBox::SetFontSize)
                .Func("setPreviewText", &InputBox::SetPreviewText)
                .Func("setText", &InputBox::SetText)
                .Func("getText", &InputBox::GetText);

            scriptEngine.Namespace("UI")
                .Func("Textbox", [this](const std::string &name)
                {
                    InputBox::Ptr textbox = InputBox::Create(defaultFont);
                    textbox->SetName(name);
                    components.push_back(textbox);
                    return textbox.get();
                });

            scriptEngine.Class<Graphics::TextMetrics>("TextMetrics")
                .Var("width", &Graphics::TextMetrics::width)
                .Var("height", &Graphics::TextMetrics::height);

            scriptEngine.Namespace("UI")
                .Func("measureText", [&loader](const std::string &fontName, const std::string &text, float fontSize) -> Graphics::TextMetrics
                {
                    auto font = loader.Load<Graphics::Font>(fontName);
                    if(font)
                        return font->MeasureString(text, fontSize);
                    return { 0, 0 };
                })
                .Func("registerMenu", [this](const std::string &menuName, Component *component)
                {
                    auto comp = std::find_if(std::begin(components), std::end(components), 
                        [&component](const Component::Ptr &elem) { return component == elem.get(); });
                    if(comp != std::end(components))
                        menus.insert_or_assign(menuName, *comp);
                })
                .Func("menuExists", [this](const std::string &menuName)
                {
                    return menus.count(menuName) > 0;
                })
                .Func("getMenu", [this](const std::string &menuName) -> Component*
                {
                    if(menus.count(menuName) > 0)
                        return menus.at(menuName).get();
                    return nullptr;
                })
                .Func("getActiveMenu", [this]()
                {
                    return activeMenu.get();
                })
                .Func("setActiveMenu", [this](const std::string &menuName)
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

            */
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

        int nFrames = 0;
        float lastTime = 0;

        void Userinterface::Update(const Timer &timer)
        {
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
            
            if(activeMenu)
                activeMenu->Update(timer);
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

        void Userinterface::RegisterMenu(std::string name, Component::Ptr component)
        {
            menus.insert_or_assign(name, component);
        }

        void Userinterface::SetActiveMenu(std::string name)
        {
            if(menus.count(name) > 0)
            {
                auto& comp = menus.at(name);
                activeMenu = comp;
            }
            else
            {
                activeMenu = nullptr;
            }
        }

        bool Userinterface::OnChar(uint32_t character)
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
            if(console.IsOpen())
            {
                console.OnMouseWheel(x, y);
                return true;
            }
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

        bool Userinterface::OnKeyRepeat(Input::InputKey key)
        {
            if(console.IsOpen())
            {
                console.OnKeyRepeat(key);
                return true;
            }
            if(activeMenu)
            {
                activeMenu->OnKeyRepeat(key);
                return true;
            }
            return false;
        }
    }
}
