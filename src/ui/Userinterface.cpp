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

#include <fameta/counter.hpp>

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
            input.RegisterChar("ui", [this](char ch){ OnChar(ch); });
            input.RegisterMouse("ui", [this](float x, float y){ OnMouseMove(x, y); });
            input.RegisterButtonDown("ui", [this](Input::InputButton button){ OnMouseDown(button); });
            input.RegisterButtonUp("ui", [this](Input::InputButton button){ OnMouseUp(button); });
            input.RegisterKeyDown("ui", [this](Input::InputKey key){ OnKeyDown(key); });
        }

        void Userinterface::Invalidate()
        {
            auto &scriptEngine = GetScriptEngine();
            scriptEngine.CallFunction<void>("InitUserinterfaces");
        }

        void Userinterface::RegisterScriptFunctions()
        {
            constexpr fameta::counter<__COUNTER__, 200> c;

            auto &scriptEngine = GetScriptEngine();
            auto &loader = GetLoader();

            scriptEngine.Register<void(const char*), c.next<__COUNTER__>()>("C_Print", [this](const char *msg){ console.Print(msg); });
            scriptEngine.Register<void(), c.next<__COUNTER__>()>("C_OpenConsole", [this](){ console.Open(); });
            scriptEngine.Register<void(), c.next<__COUNTER__>()>("C_CloseConsole", [this](){ console.Close(); });
            scriptEngine.Register<void(), c.next<__COUNTER__>()>("C_ToggleConsole", [this](){ console.Toggle(); });

            scriptEngine.Register<float(), c.next<__COUNTER__>()>("UI_GetWidth", [this](){ return static_cast<float>(uiWidth); });
            scriptEngine.Register<float(), c.next<__COUNTER__>()>("UI_GetHeight", [this](){ return static_cast<float>(uiHeight); });

            scriptEngine.Register<void(), c.next<__COUNTER__>()>("UI_ClearRoot", [this]()
            {
                rootContainer->RemoveAll();
            });

            scriptEngine.Register<void(void*), c.next<__COUNTER__>()>("UI_AddToRoot", [this](void *component)
            {
                auto comp = std::find_if(std::begin(components), std::end(components), 
                    [&component](const ComponentPtr &elem) { return static_cast<Component*>(component) == elem.get(); });
                if(comp != std::end(components))
                    rootContainer->Add(*comp);
            });

            scriptEngine.Register<void*(const char*), c.next<__COUNTER__>()>("UI_CreateImage", [this](const char *name)
            {
                ImagePtr image = std::make_shared<Image>();
                image->SetName(name);
                components.push_back(image);
                return image.get();
            });

            scriptEngine.Register<void(void*, float, float), c.next<__COUNTER__>()>("UI_ImageSetPosition", [this](void *image, float x, float y)
            {
                Image *i = static_cast<Image*>(image);
                i->SetPosition({x, y});
            });

            scriptEngine.Register<void(void*, float, float), c.next<__COUNTER__>()>("UI_ImageSetSize", [this](void *image, float w, float h)
            {
                Image *i = static_cast<Image*>(image);
                i->SetSize({w, h});
            });

            scriptEngine.Register<void(void*, const char*), c.next<__COUNTER__>()>("UI_ImageSetImage", [this, &loader](void *image, const char *textureName)
            {
                Image *i = static_cast<Image*>(image);
                auto texture = loader.Load<Graphics::Texture>(textureName);
                i->SetImage(texture);
            });

            scriptEngine.Register<void*(const char*), c.next<__COUNTER__>()>("UI_CreateButton", [this](const char *name)
            {
                ButtonPtr button = std::make_shared<Button>(defaultFont);
                button->SetName(name);
                components.push_back(button);
                return button.get();
            });

            scriptEngine.Register<void(void*, float, float), c.next<__COUNTER__>()>("UI_ButtonSetPosition", [this](void *button, float x, float y)
            {
                Button *b = static_cast<Button*>(button);
                b->SetPosition({x, y});
            });

            scriptEngine.Register<void(void*, float, float), c.next<__COUNTER__>()>("UI_ButtonSetSize", [this](void *button, float w, float h)
            {
                Button *b = static_cast<Button*>(button);
                b->SetSize({w, h});
            });

            scriptEngine.Register<void(void*, const char*), c.next<__COUNTER__>()>("UI_ButtonSetText", [this](void *button, const char *text)
            {
                Button *b = static_cast<Button*>(button);
                b->SetText(text);
            });

            scriptEngine.Register<void(void*, const char*), c.next<__COUNTER__>()>("UI_ButtonSetFont", [this, &loader](void *button, const char *fontName)
            {
                Button *b = static_cast<Button*>(button);
                auto font = loader.Load<Graphics::Font>(fontName);
                if(font)
                {
                    b->SetFont(font);
                }
            });

            scriptEngine.Register<void(void*, float), c.next<__COUNTER__>()>("UI_ButtonSetFontSize", [this](void *button, float fontSize)
            {
                Button *b = static_cast<Button*>(button);
                b->SetFontSize(fontSize);
            });

            scriptEngine.Register<void(void*, void(*)()), c.next<__COUNTER__>()>("UI_ButtonSetCallback", [this](void *button, void(*btnCallback)())
            {
                Button *b = static_cast<Button*>(button);
                b->SetCallback(btnCallback);
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
            rootContainer->Draw(*renderer, glm::vec2());
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
