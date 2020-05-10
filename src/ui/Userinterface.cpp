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
            textRenderer = std::make_unique<Graphics::TextRenderer>();

            console.InitLimits(glm::vec2(uiWidth, uiHeight));

            auto &loader = GetLoader();
            defaultFont = loader.Load<Graphics::Font>("fonts/unispace.json");

            Config &config = GetConfig();
            uiWidth = config.GetValue("r_width", 800);
            uiHeight = config.GetValue("r_height", 600);

            fpsLabel = std::make_shared<Label>(defaultFont);
            fpsLabel->SetPosition({0, uiHeight - 22});
            fpsLabel->SetText("0");
            fpsLabel->SetTextColor({1, 1, 1, 1});
            fpsLabel->SetFont(defaultFont, 16);

            console.BindFunc("fps", Helpers::BoolFunc(showFps, "Show FPS", "Hide FPS"));
            console.BindFunc("frametime", Helpers::BoolFunc(showFrametime, "Show Frametime", "Hide Frametime"));

            auto &input = GetInput();
            input.RegisterChar("ui", [this](char ch){ OnChar(ch); });
            input.RegisterMouse("ui", [this](float x, float y){ OnMouseMove(x, y); });
            input.RegisterButtonDown("ui", [this](Input::InputButton button){ OnMouseDown(button); });
            input.RegisterButtonUp("ui", [this](Input::InputButton button){ OnMouseUp(button); });
            input.RegisterKeyDown("ui", [this](Input::InputKey key){ OnKeyDown(key); });

            auto &scriptEngine = GetScriptEngine();

            scriptEngine.Register([this](const char *str){ console.Print(str); }, "print");
            scriptEngine.Register([this](){ console.Toggle(); }, "console", "toggle");
            scriptEngine.Register([this](){ console.Open(); }, "console", "open");
            scriptEngine.Register([this](){ console.Close(); }, "console", "close");

            scriptEngine.Register(uiWidth, "ui", "width");
            scriptEngine.Register(uiHeight, "ui", "height");

            scriptEngine.Register([&, this](const char *layoutName)
            {
                int id = currentId++;
                namedLayouts.insert_or_assign(std::string(layoutName), id);

                PanelPtr container = std::make_shared<Panel>();
                layouts.insert_or_assign(id, container);

                auto &state = scriptEngine.GetState();
                state.set("tmp", Script::LTable());
                Script::LValue& tmp = state["tmp"];
                tmp.set("id", id);

                tmp.set("createButton", [this, &state, container](const char *name)
                {
                    state.set("btntmp", Script::LTable());
                    Script::LValue& btn = state["btntmp"];
                    btn.set("name", name);

                    ButtonPtr button = std::make_shared<Button>(defaultFont);
                    button->SetName(name);
                    container->Add(button);

                    btn.set("setText",          [button](const char *text){ button->SetText(text); });
                    btn.set("setPosition",      [button](float x, float y){ button->SetPosition({x, y}); });
                    btn.set("setSize",          [button](float w, float h){ button->SetSize({w, h}); });
                    //btn.set("setFont",          [button](int fontId, float size){ button->SetFont(fontId, size); });
                    btn.set("setTextColor",     [button](float r, float g, float b, float a){ button->SetTextColor({r, g, b, a}); });
                    btn.set("setNormalColor",   [button](float r, float g, float b, float a){ button->SetNormalColor({r, g, b, a}); });
                    btn.set("setNormalImage",   [button](int imageId){ button->SetNormalImage(imageId); });
                    btn.set("setHoverColor",    [button](float r, float g, float b, float a){ button->SetHoverColor({r, g, b, a}); });
                    btn.set("setHoverImage",    [button](int imageId){ button->SetHoverImage(imageId); });
                    btn.set("setDownColor",     [button](float r, float g, float b, float a){ button->SetDownColor({r, g, b, a}); });
                    btn.set("setDownImage",     [button](int imageId){ button->SetDownImage(imageId); });

                    btn.set("setCallback", [this, &state, button](Script::LValue func)
                    {
                        if(!func) return;
                        state.set((button->GetName() + "func").c_str(), func);
                        button->SetCallback([this, button, &state]()
                        { 
                            try
                            {
                                state[(button->GetName() + "func").c_str()].call();
                            }
                            catch(const std::exception &e)
                            {
                                console.Print(e.what());
                                Logger::Instance().Error(e.what());
                            }
                        });
                    });

                    return btn;
                });

                tmp.set("createLabel", [this, &state, container](const char *name)
                {
                    state.set("lbltmp", Script::LTable());
                    Script::LValue &lbl = state["lbltmp"];
                    lbl.set("name", name);

                    LabelPtr label = std::make_shared<Label>(defaultFont);
                    label->SetName(name);
                    container->Add(label);

                    lbl.set("setText",      [label](const char *text){ label->SetText(text); });
                    lbl.set("setPosition",  [label](float x, float y){ label->SetPosition({ x, y }); });
                    lbl.set("setVisible",   [label](bool visible){ label->SetVisible(visible); });
                    lbl.set("setTextColor", [label](float r, float g, float b, float a){ label->SetTextColor({ r, g, b, a }); });
                    //lbl.set("setFont",      [label](int fontId, float size){ label->SetFont(fontId, size); });

                    return lbl;
                });

                tmp.set("createImage", [this, &state, container](const char *name)
                {
                    state.set("imgtmp", Script::LTable());
                    Script::LValue &img = state["imgtmp"];
                    img.set("name", name);

                    ImagePtr image = std::make_shared<Image>();
                    image->SetName(name);
                    container->Add(image);

                    img.set("setImage", [image](int imageId){ image->SetImage(imageId); });
                    img.set("setColor", [image](float r, float g, float b, float a){ image->SetColor({ r, g, b, a }); });
                    img.set("setPosition", [image](float x, float y){ image->SetPosition({ x, y, }); });
                    img.set("setSize", [image](float w, float h){ image->SetSize({ w, h }); });

                    return img;
                });

                return tmp;
            }, "ui", "createLayout");

            scriptEngine.Register([this](Script::LValue layoutValue)
            { 
                int id = layoutValue["id"];
                ContainerPtr container = layouts.at(id);
                rootContainer = container;
            }, "ui", "setLayout");
        }

        Console &Userinterface::GetConsole()
        {
            return console;
        }

        void Userinterface::LoadLayout(const std::string &layout)
        {
            
        }

        void Userinterface::Draw()
        {
            textRenderer->Begin(static_cast<float>(uiWidth), static_cast<float>(uiHeight));
            if(showFps)
                fpsLabel->Draw(*textRenderer.get(), glm::vec2());
            rootContainer->Draw(*textRenderer.get(), glm::vec2());
            console.Draw(*textRenderer.get());
            textRenderer->End();
        }

        void Userinterface::Update(const Timer &timer)
        {
            frameTime = timer.Delta();
            if(showFrametime)
                fpsLabel->SetText(std::to_string(frameTime));
            else
                fpsLabel->SetText(std::to_string(static_cast<int>(1 / frameTime)));
            rootContainer->Update();
            console.Update(timer);
        }

        void Userinterface::OnChar(char character)
        {
            rootContainer->OnChar(character);
            console.OnChar(character);
        }

        void Userinterface::OnMouseMove(float x, float y)
        {
            y = uiHeight - y;
            rootContainer->OnMouseMove(x, y);
        }

        void Userinterface::OnMouseDown(Input::InputButton button)
        {
            rootContainer->OnMouseDown(button);
        }

        void Userinterface::OnMouseUp(Input::InputButton button)
        {
            rootContainer->OnMouseUp(button);
        }

        void Userinterface::OnKeyDown(Input::InputKey key)
        {
            console.OnKeyDown(key);
        }
    }
}
