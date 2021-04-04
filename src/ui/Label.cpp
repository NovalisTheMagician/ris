#include "ui/Label.hpp"

#include <string>

using std::string;

namespace RIS::UI
{
    Label::Label(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont)
        : parentFramebuffer(parentFramebuffer), font(defaultFont)
    {}

    Label& Label::SetName(const std::string &name)
    { 
        this->name = name; return *this; 
    }

    std::string Label::GetName() const 
    { 
        return name; 
    }

    Label& Label::SetAnchor(Anchor anchor) 
    { 
        this->anchor = anchor; 
        return *this; 
    }

    Anchor Label::GetAnchor() const 
    { 
        return anchor; 
    }

    Label& Label::SetPosition(const glm::vec2 &position) 
    { 
        this->position = position; 
        return *this; 
    }

    Label& Label::SetSize(const glm::vec2 &size) 
    { 
        this->size = size; 
        return *this; 
    }

    Label& Label::SetFont(std::shared_ptr<Graphics::Font> font)
    {
        this->font = font;
        return *this;
    }

    Label& Label::SetFontSize(float fontSize)
    {
        this->fontSize = fontSize;
        return *this;
    }

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

    void Label::Update(const Timer &timer)
    {

    }

    void Label::Draw(Graphics::SpriteRenderer &renderer)
    {
        renderer.DrawString(text, *font.get(), fontSize, position, fontColor);
    }
}
