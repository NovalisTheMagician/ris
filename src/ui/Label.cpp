#include "ui/Label.hpp"

#include <string>

using std::string;

namespace RIS::UI
{
    Label::Label(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize)
        : Component(parentFramebuffer, defaultFont, parentSize)
    {}

    Label& Label::SetTextColor(const glm::vec4 &color)
    {
        fontColor = color;
        return *this;
    }

    Label& Label::SetVisible(bool visible)
    {
        isVisible = visible;
        return *this;
    }

    Label& Label::SetText(const string &text)
    {
        this->text = text;
        return *this;
    }

    void Label::Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset)
    {
        glm::vec2 pos = GetAnchoredPosition() + offset;
        renderer.DrawString(text, *font.get(), fontSize, pos, fontColor);
    }
}
