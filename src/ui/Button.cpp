#include "ui/Button.hpp"

#include "misc/Logger.hpp"

#include <iostream>

namespace RIS::UI
{
    Button::Button(Graphics::Framebuffer &parentFramebuffer, std::shared_ptr<Graphics::Font> defaultFont, const glm::vec2 &parentSize)
        : Component(parentFramebuffer, defaultFont, parentSize), callback([](Button&){})
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

    glm::vec4 Button::GetTextColor() const
    {
        return textColor;
    }

    Button& Button::SetCallback(ButtonFunc func)
    {
        callback = func;
        return *this;
    }

    Button& Button::SetToggleMode(bool isToggle)
    {
        this->isToggle = isToggle;
        return *this;
    }

    bool Button::IsToggleMode() const
    {
        return isToggle;
    }

    Button& Button::SetToggle(bool isToggle)
    {
        this->toggleOn = isToggle;
        return *this;
    }

    bool Button::IsToggle() const
    {
        return toggleOn;
    }

    Button& Button::SetNormalTexture(Graphics::Texture::Ptr normalTexture)
    {
        this->normalImage = normalTexture;
        return *this;
    }

    Button& Button::SetHoverTexture(Graphics::Texture::Ptr hoverTexture)
    {
        this->hoverImage = hoverTexture;
        return *this;
    }

    Button& Button::SetDownTexture(Graphics::Texture::Ptr downTexture)
    {
        this->downImage = downTexture;
        return *this;
    }

    Button& Button::SetTextures(const ButtonTextures &textures)
    {
        normalImage = textures.normal;
        hoverImage = textures.hover;
        downImage = textures.click;
        inactiveImage = textures.disabled;
        return *this;
    }

    Button& Button::SetContinuous(bool isContinuous)
    {
        this->isContinuous = isContinuous;
        return *this;
    }

    bool Button::IsContinuous() const
    {
        return isContinuous;
    }

    void Button::Update(const Timer &timer)
    {
        if(isClickedDown && isContinuous)
        {
            continuousTimeout += timer.Delta();

            if(continuousTimeout >= continuousTick)
            {
                continuousTimeout -= continuousTick;
                callback(*this);
            }
        }
    }

    void Button::Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset)
    {
        if(!visible) return;

        glm::vec2 pos = GetAnchoredPosition() + offset;

        Graphics::TextMetrics metrics = font->MeasureString(text, fontSize);
        glm::vec2 textPos = pos + ((size / glm::vec2(2)) - (glm::vec2(metrics.width, metrics.height) / glm::vec2(2)));

        glm::vec4 color = normalColor;
        Graphics::Texture::Ptr image = normalImage;

        if(!active)
        {
            glm::vec4 inactiveColor(glm::vec3(0.7f), 1.0f);
            if(inactiveImage)
                renderer.DrawTexture(*inactiveImage, pos, size);
            else
                renderer.DrawRect(pos, size, inactiveColor);
            renderer.DrawString(text, *font.get(), fontSize, textPos, textColor);
            return;
        }

        if(isClickedDown || (isToggle && toggleOn))
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
            renderer.DrawTexture(*image, pos, size);
        else
            renderer.DrawRect(pos, size, color);
        renderer.DrawString(text, *font.get(), fontSize, textPos, textColor);
    }

    void Button::OnMouseMove(float x, float y)
    {
        if(!visible) return;
        glm::vec2 pos = GetAnchoredPosition();

        if( x > pos.x && x < pos.x + size.x &&
            y > pos.y && y < pos.y + size.y)
            isInBounds = true;
        else 
            isInBounds = false;
    }

    void Button::OnMouseDown(Input::InputKey button)
    {
        if(!visible) return;
        if(button == Input::InputKey::MOUSE_LEFT && isInBounds)
        {
            isClickedDown = true;
        }
    }

    void Button::OnMouseUp(Input::InputKey button)
    {
        if(!visible) return;
        if(button == Input::InputKey::MOUSE_LEFT && isClickedDown)
        {
            if(active && isInBounds && !isContinuous)
            {
                toggleOn = !toggleOn;
                callback(*this);
            }
            isClickedDown = false;
        }
    }
}
