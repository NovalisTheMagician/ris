#include "ui/Button.hpp"

#include "misc/Logger.hpp"

#include <iostream>

namespace RIS::UI
{
    Button::Button(Graphics::Framebuffer &parentFramebuffer, std::shared_ptr<Graphics::Font> defaultFont)
        : parentFramebuffer(parentFramebuffer), font(defaultFont), callback([](){})
    {}

    Button& Button::SetName(const std::string &name) 
    { 
        this->name = name; return *this; 
    }

    std::string Button::GetName() const 
    { 
        return name; 
    }

    Button& Button::SetAnchor(Anchor anchor) 
    { 
        this->anchor = anchor; 
        return *this; 
    }

    Anchor Button::GetAnchor() const 
    { 
        return anchor; 
    }

    Button& Button::SetPosition(const glm::vec2 &position) 
    { 
        this->position = position; 
        return *this; 
    }

    Button& Button::SetSize(const glm::vec2 &size) 
    { 
        this->size = size; 
        return *this; 
    }

    Button& Button::SetText(const std::string &text)
    {
        this->text = text;
        return *this;
    }

    Button& Button::SetFont(std::shared_ptr<Graphics::Font> font)
    {
        this->font = font;
        return *this;
    }

    Button& Button::SetFontSize(float size)
    {
        this->fontSize = size;
        return *this;
    }

    Button& Button::SetTextColor(const glm::vec4 &color)
    {
        this->textColor = color;
        return *this;
    }

    Button& Button::SetCallback(ButtonFunc func)
    {
        callback = func;
        return *this;
    }

    Button& Button::SetActive(bool isActive)
    {
        active = isActive;
        return *this;
    }

    void Button::Update(const Timer &timer)
    {
        
    }

    void Button::Draw(Graphics::SpriteRenderer &renderer)
    {
        Graphics::TextMetrics metrics = font->MeasureString(text, fontSize);
        glm::vec2 textPos = position + ((size / glm::vec2(2)) - (glm::vec2(metrics.width, metrics.height) / glm::vec2(2)));

        glm::vec4 color = normalColor;
        std::shared_ptr<Graphics::Texture> image = normalImage;

        if(!active)
        {
            glm::vec4 inactiveColor(0.7f);
            if(image)
                renderer.DrawTexture(*image, position, size, inactiveColor);
            else
                renderer.DrawRect(position, size, inactiveColor);
            renderer.DrawString(text, *font.get(), fontSize, textPos, textColor);
            return;
        }

        if(isClickedDown)
        {
            color = downColor;
            image = downImage;
        }
        else if(isInBounds)
        {
            color = hoverColor;
            image = hoverImage;
        }

        if(image)
            renderer.DrawTexture(*image, position, size, color);
        else
            renderer.DrawRect(position, size, color);
        renderer.DrawString(text, *font.get(), fontSize, textPos, textColor);
    }

    void Button::OnMouseMove(float x, float y)
    {
        if( x > position.x && x < position.x + size.x &&
            y > position.y && y < position.y + size.y)
            isInBounds = true;
        else 
            isInBounds = false;
    }

    void Button::OnMouseDown(Input::InputKey button)
    {
        if(button == Input::InputKey::MOUSE_LEFT && isInBounds)
        {
            isClickedDown = true;
        }
    }

    void Button::OnMouseUp(Input::InputKey button)
    {
        if(button == Input::InputKey::MOUSE_LEFT && isClickedDown)
        {
            if(active && isInBounds)
            {
                callback();
            }
            isClickedDown = false;
        }
    }
}
