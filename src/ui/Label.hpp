#pragma once

#include "ui/Component.hpp"

#include "graphics/Font.hpp"

#include <glm/glm.hpp>

#include <string>

namespace RIS::UI
{
    class Label : public Component<Label>
    {
    public:
        Label(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize);
        Label(const Label&) = delete;
        Label& operator=(const Label&) = delete;
        Label(Label&&) = default;
        Label& operator=(Label&&) = default;

        Label& SetTextColor(const glm::vec4 &color);
        glm::vec4 GetTextColor() const;
        Label& SetText(const std::string &text);
        std::string GetText() const;

        glm::vec2 GetSize() const;

        void Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset);

    private:
        std::string text = "";

        glm::vec4 fontColor = glm::vec4(1, 1, 1, 1);

    };
}
