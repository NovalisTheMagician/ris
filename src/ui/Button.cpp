#include "ui/Button.hpp"

#include "misc/Logger.hpp"

#include <iostream>

namespace RIS::UI
{
    Button::Button(Graphics::Framebuffer &parentFramebuffer, std::shared_ptr<Graphics::Font> defaultFont, const glm::vec2 &parentSize)
        : Component(parentFramebuffer, defaultFont, parentSize), callback([](){})
    {}

    Button& Button::SetText(const std::string &text)
    {
        this->text = text;
        return *this;
    }

    std::string Button::GetText() const
    {
        return text;
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
