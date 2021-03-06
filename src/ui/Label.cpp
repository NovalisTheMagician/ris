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

    glm::vec4 Label::GetTextColor() const
    {
        return fontColor;
    }

    Label& Label::SetText(const string &text)
    {
        this->text = text;
        return *this;
    }

    std::string Label::GetText() const
    {
        return text;
    }

    glm::vec2 Label::GetSize() const
    {
        auto metrics = font->MeasureString(text, fontSize);
        return {metrics.width, metrics.height};
    }

    void Label::Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset)
    {
        if(!visible) return;

        glm::vec2 pos = GetAnchoredPosition() + offset;
        renderer.DrawString(text, *font.get(), fontSize, pos, fontColor);
    }
}
