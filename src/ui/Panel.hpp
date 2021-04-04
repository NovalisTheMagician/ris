#pragma once

#include "ui/Component.hpp"

#include "graphics/Texture.hpp"
#include "graphics/Framebuffer.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <variant>
#include <vector>

namespace RIS::UI
{
    class Button;
    class Image;
    class Inputbox;
    class Label;

    class Panel
    {
    private:
        using UITypes = std::variant<Button, Image, Inputbox, Label, Panel>;

    public:
        Panel(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont);
        virtual ~Panel() = default;
        Panel(const Panel&) = delete;
        Panel& operator=(const Panel&) = delete;
        Panel(Panel&&) = default;
        Panel& operator=(Panel&&) = default;

        Panel& SetName(const std::string &name);
        std::string GetName() const;
        Panel& SetAnchor(Anchor anchor);
        Anchor GetAnchor() const;
        Panel& SetPosition(const glm::vec2 &position);
        Panel& SetSize(const glm::vec2 &size);
        Panel& SetColor(const glm::vec4 &color);

        void OnChar(uint32_t c);
        void OnMouseMove(float x, float y);
        void OnMouseDown(Input::InputKey mouseCode);
        void OnMouseUp(Input::InputKey mouseCode);
        void OnMouseWheel(float x, float y);

        void OnKeyDown(Input::InputKey keyCode);
        void OnKeyUp(Input::InputKey keyCode);
        void OnKeyRepeat(Input::InputKey keyCode);

        void Update(const Timer &timer);
        void Draw(Graphics::SpriteRenderer &renderer);

        Button& CreateButton();
        Label& CreateLabel();
        Image& CreateImage();
        Inputbox& CreateInputbox();

    private:
        std::string name;
        glm::vec2 position;
        glm::vec2 size = glm::vec2(64, 32);
        Anchor anchor = Anchor::TopLeft;
        Graphics::Framebuffer &parentFramebuffer;
        Graphics::Font::Ptr font;

        std::vector<UITypes> components;

        glm::vec4 color;

        Graphics::Texture::Ptr backgroundImage;
        Graphics::Framebuffer panelFramebuffer;

    };
}
