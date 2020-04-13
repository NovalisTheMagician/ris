#include "Button.hpp"

#include "common/Logger.hpp"

#include <iostream>

namespace RIS
{
    UIButton::UIButton(const SystemLocator &systems)
        : Component(systems)
    {
        callback = [](){};
    }

    UIButton::~UIButton()
    {
    }

    void UIButton::SetPosition(const glm::vec2 &position)
    {
        this->position = position;
    }

    void UIButton::SetSize(const glm::vec2 &size)
    {
        this->size = size;
    }

    void UIButton::SetText(const std::string &text)
    {
        this->text = text;
    }

    void UIButton::SetFont(int font, float size)
    {
        this->font = font;
        this->fontSize = size;
    }

    void UIButton::SetTextColor(const glm::vec4 &color)
    {
        this->textColor = color;
    }

    void UIButton::SetColors(const glm::vec4 &normal, const glm::vec4 &hover, const glm::vec4 &down)
    {
        normalColor = normal;
        hoverColor = hover;
        downColor = down;
    }

    void UIButton::SetImages(int normal, int hover, int down)
    {
        normalImage = normal;
        hoverImage = hover;
        downImage = down;
    }

    void UIButton::SetNormalColor(const glm::vec4 &color)
    {
        normalColor = color;
    }

    void UIButton::SetNormalImage(int image)
    {
        normalImage = image;
    }

    void UIButton::SetHoverColor(const glm::vec4 &color)
    {
        hoverColor = color;
    }

    void UIButton::SetHoverImage(int image)
    {
        hoverImage = image;
    }

    void UIButton::SetDownColor(const glm::vec4 &color)
    {
        downColor = color;
    }

    void UIButton::SetDownImage(int image)
    {
        downImage = image;
    }

    void UIButton::SetCallback(ButtonFunc func)
    {
        callback = func;
    }

    void UIButton::Update()
    {
        
    }

    void UIButton::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        parentPos = parentPosition;

        glm::vec2 pos = parentPosition + position;
        TextMetrics metrics = renderer.MeasureText(text, font, fontSize);
        glm::vec2 textPos = pos + ((size / glm::vec2(2)) - (glm::vec2(metrics.width, metrics.height) / glm::vec2(2)));

        glm::vec4 color = normalColor;
        int image = normalImage;

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

        renderer.SetTexture(image, 0);
        renderer.DrawQuad(pos, size, color);
        renderer.DrawText(text, font, textPos, fontSize, textColor);
    }

    void UIButton::OnMouseMove(float x, float y)
    {
        glm::vec2 pos = parentPos + position;

        if( x > pos.x && x < pos.x + size.x &&
            y > pos.y && y < pos.y + size.y)
            isInBounds = true;
        else 
            isInBounds = false;
    }

    void UIButton::OnMouseDown(InputButton button)
    {
        if(button == InputButton::LEFT && isInBounds)
        {
            isClickedDown = true;
        }
    }

    void UIButton::OnMouseUp(InputButton button)
    {
        auto &window = systems.GetWindow();

        if(button == InputButton::LEFT && isClickedDown)
        {
            if(isInBounds)
            {
                callback();
            }
            isClickedDown = false;
        }
    }
}
