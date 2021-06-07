#pragma once

#include "ui/Component.hpp"
#include "ui/Button.hpp"
#include "ui/Image.hpp"
#include "ui/Label.hpp"
#include "ui/Inputbox.hpp"

#include "graphics/Texture.hpp"
#include "graphics/Framebuffer.hpp"
#include "graphics/Colors.hpp"

#include <glm/glm.hpp>

#include <string>
#include <variant>
#include <vector>
#include <list>
#include <optional>
#include <functional>

namespace RIS::UI
{
    using CallbackFunc = std::function<void()>;

    class Panel : public Component<Panel>
    {
    private:
        using UITypes = std::variant<Button, Image, Inputbox, Label, Panel>;

    public:
        Panel(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, glm::vec2 parentSize);
        Panel(const Panel&) = delete;
        Panel& operator=(const Panel&) = delete;
        Panel(Panel&&) = default;
        Panel& operator=(Panel&&) = default;

        Panel& SetSize(const glm::vec2 &size);
        Panel& SetColor(const glm::vec4 &color);

        Panel& SetActive(bool active);

        Panel& SetBackAction(CallbackFunc func);
        Panel& SetBackButton(Input::InputKey backKey);
        Input::InputKey GetBackButton() const;

        void OnChar(uint32_t c);
        void OnMouseMove(float x, float y);
        void OnMouseDown(Input::InputKey mouseCode);
        void OnMouseUp(Input::InputKey mouseCode);
        void OnMouseWheel(float x, float y);

        void OnKeyDown(Input::InputKey keyCode);
        void OnKeyUp(Input::InputKey keyCode);
        void OnKeyRepeat(Input::InputKey keyCode);

        void Reset();

        void Update(const Timer &timer);
        void Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset);

        template<typename Comp>
        std::optional<std::reference_wrapper<Comp>> GetComponent(const std::string &name)
        {
            for(auto &v : components)
            {
                if(auto comp = std::get_if<Comp>(&v))
                {
                    Comp &component = *comp;
                    if(component.GetName() == name)
                        return std::ref(component);
                }
            }
            return std::nullopt;
        }

        Button& CreateButton();
        Label& CreateLabel();
        Image& CreateImage();
        Inputbox& CreateInputbox();
        Panel& CreatePanel();

    private:
        std::list<UITypes> components;

        glm::vec4 color = Graphics::Colors::Transparent;

        Graphics::Texture::Ptr backgroundImage;
        Graphics::Framebuffer panelFramebuffer;

        CallbackFunc backCallback;
        Input::InputKey backKey = Input::InputKey::ESCAPE;

    };
}
