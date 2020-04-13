#include "SimpleUserinterface.hpp"

#include "common/IRenderer.hpp"
#include "common/ILoader.hpp"
#include "common/IAudio.hpp"
#include "common/IInput.hpp"
#include "common/LuaScriptEngine.hpp"

#include "common/Logger.hpp"

#include <algorithm>

#include "common/StringSupport.hpp"
#include "common/MathHelper.hpp"

using namespace std::literals;

namespace RIS
{
    SimpleUserinterface::SimpleUserinterface(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), console(systems), 
            uiWidth(config.GetValue("r_width", 800)), uiHeight(config.GetValue("r_height", 600))
    {
        rootContainer = MakePanel(systems);
    }

    SimpleUserinterface::~SimpleUserinterface()
    {
    }

    void SimpleUserinterface::PostInit()
    {
        console.InitLimits(glm::vec2(uiWidth, uiHeight));

        IRenderer &renderer = systems.GetRenderer();
        uiFramebufferId = renderer.CreateFramebuffer(uiWidth, uiHeight, true);

        IInput &input = systems.GetInput();
        input.RegisterChar("ui", [this](char ch){ OnChar(ch); });
        input.RegisterMouse("ui", [this](float x, float y){ OnMouseMove(x, y); });
        input.RegisterButtonDown("ui", [this](InputButton button){ OnMouseDown(button); });
        input.RegisterButtonUp("ui", [this](InputButton button){ OnMouseUp(button); });
        input.RegisterKeyDown("ui", [this](InputKey key){ OnKeyDown(key); });

        auto &scriptEngine = dynamic_cast<LuaScriptEngine&>(systems.GetScriptEngine());

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

            PanelPtr container = MakePanel(systems);
            layouts.insert_or_assign(id, container);

            auto &state = scriptEngine.GetState();
            state.set("tmp", LTable());
            LValue& tmp = state["tmp"];
            tmp.set("id", id);

            tmp.set("createButton", [this, &state, container](const char *name)
            {
                state.set("btntmp", LTable());
                LValue& btn = state["btntmp"];
                btn.set("name", name);

                ButtonPtr button = MakeButton(systems);
                button->SetName(name);
                container->Add(button);

                btn.set("setText",          [button](const char *text){ button->SetText(text); });
                btn.set("setPosition",      [button](float x, float y){ button->SetPosition({x, y}); });
                btn.set("setSize",          [button](float w, float h){ button->SetSize({w, h}); });
                btn.set("setFont",          [button](int fontId, float size){ button->SetFont(fontId, size); });
                btn.set("setTextColor",     [button](float r, float g, float b, float a){ button->SetTextColor({r, g, b, a}); });
                btn.set("setNormalColor",   [button](float r, float g, float b, float a){ button->SetNormalColor({r, g, b, a}); });
                btn.set("setNormalImage",   [button](int imageId){ button->SetNormalImage(imageId); });
                btn.set("setHoverColor",    [button](float r, float g, float b, float a){ button->SetHoverColor({r, g, b, a}); });
                btn.set("setHoverImage",    [button](int imageId){ button->SetHoverImage(imageId); });
                btn.set("setDownColor",     [button](float r, float g, float b, float a){ button->SetDownColor({r, g, b, a}); });
                btn.set("setDownImage",     [button](int imageId){ button->SetDownImage(imageId); });

                btn.set("setCallback", [this, &state, button](LValue func)
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
                state.set("lbltmp", LTable());
                LValue &lbl = state["lbltmp"];
                lbl.set("name", name);

                LabelPtr label = MakeLabel(systems);
                label->SetName(name);
                container->Add(label);

                lbl.set("setText",      [label](const char *text){ label->SetText(text); });
                lbl.set("setPosition",  [label](float x, float y){ label->SetPosition({ x, y }); });
                lbl.set("setVisible",   [label](bool visible){ label->SetVisible(visible); });
                lbl.set("setTextColor", [label](float r, float g, float b, float a){ label->SetTextColor({ r, g, b, a }); });
                lbl.set("setFont",      [label](int fontId, float size){ label->SetFont(fontId, size); });

                return lbl;
            });

            tmp.set("createImage", [this, &state, container](const char *name)
            {
                state.set("imgtmp", LTable());
                LValue &img = state["imgtmp"];
                img.set("name", name);

                ImagePtr image = MakeImage(systems);
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

        scriptEngine.Register([this](LValue layoutValue)
        { 
            int id = layoutValue["id"];
            ContainerPtr container = layouts.at(id);
            rootContainer = container;
        }, "ui", "setLayout");
    }

    IConsole &SimpleUserinterface::GetConsole()
    {
        return console;
    }

    void SimpleUserinterface::LoadLayout(const std::string &layout)
    {
        
    }

    void SimpleUserinterface::Draw()
    {
        IRenderer &renderer = systems.GetRenderer();
        I2DRenderer &renderer2D = renderer.Get2DRenderer();

        // for intel gpus: bind the framebuffer first before clearing it
        renderer.SetFramebuffer(uiFramebufferId);
        renderer.Clear(uiFramebufferId, glm::vec4(0, 0, 0, 0));

        renderer2D.Begin();

        rootContainer->Draw(renderer2D, glm::vec2());

        console.Draw(renderer2D);

        renderer2D.End();

        renderer.Draw(uiFramebufferId);

        renderer.SetFramebuffer(0);
    }

    void SimpleUserinterface::Update(const Timer &timer)
    {
        rootContainer->Update();
        console.Update(timer);
    }

    void SimpleUserinterface::OnChar(char character)
    {
        rootContainer->OnChar(character);
        console.OnChar(character);
    }

    void SimpleUserinterface::OnMouseMove(float x, float y)
    {
        y = uiHeight - y;
        rootContainer->OnMouseMove(x, y);
    }

    void SimpleUserinterface::OnMouseDown(InputButton button)
    {
        rootContainer->OnMouseDown(button);
    }

    void SimpleUserinterface::OnMouseUp(InputButton button)
    {
        rootContainer->OnMouseUp(button);
    }

    void SimpleUserinterface::OnKeyDown(InputKey key)
    {
        console.OnKeyDown(key);
    }
}
