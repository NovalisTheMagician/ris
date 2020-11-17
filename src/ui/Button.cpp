#include "ui/Button.hpp"

#include "misc/Logger.hpp"

#include <iostream>

namespace RIS
{
    namespace UI
    {
        Button::Ptr Button::Create(std::shared_ptr<Graphics::Font> defaultFont)
        {
            return std::make_shared<Button>(defaultFont);
        }

        Button::Button(std::shared_ptr<Graphics::Font> defaultFont)
            : callback([](){}), font(defaultFont)
        {
        }

        void Button::SetText(const std::string &text)
        {
            this->text = text;
        }

        void Button::SetFont(std::shared_ptr<Graphics::Font> font)
        {
            this->font = font;
        }

        void Button::SetFontSize(float size)
        {
            this->fontSize = size;
        }

        void Button::SetTextColor(const glm::vec4 &color)
        {
            this->textColor = color;
        }

        void Button::SetColors(const glm::vec4 &normal, const glm::vec4 &hover, const glm::vec4 &down)
        {
            normalColor = normal;
            hoverColor = hover;
            downColor = down;
        }

        void Button::SetImages(std::shared_ptr<Graphics::Texture> normal, std::shared_ptr<Graphics::Texture> hover, std::shared_ptr<Graphics::Texture> down)
        {
            normalImage = normal;
            hoverImage = hover;
            downImage = down;
        }

        void Button::SetNormalColor(const glm::vec4 &color)
        {
            normalColor = color;
        }

        void Button::SetNormalImage(std::shared_ptr<Graphics::Texture> image)
        {
            normalImage = image;
        }

        void Button::SetHoverColor(const glm::vec4 &color)
        {
            hoverColor = color;
        }

        void Button::SetHoverImage(std::shared_ptr<Graphics::Texture> image)
        {
            hoverImage = image;
        }

        void Button::SetDownColor(const glm::vec4 &color)
        {
            downColor = color;
        }

        void Button::SetDownImage(std::shared_ptr<Graphics::Texture> image)
        {
            downImage = image;
        }

        void Button::SetCallback(ButtonFunc func)
        {
            callback = func;
        }

        void Button::SetActive(bool isActive)
        {
            active = isActive;
        }

        void Button::Update(const Timer &timer)
        {
            
        }

        void Button::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
        {
            glm::vec2 pos = parentPosition + position;
            Graphics::TextMetrics metrics = font->MeasureString(text, fontSize);
            glm::vec2 textPos = pos + ((size / glm::vec2(2)) - (glm::vec2(metrics.width, metrics.height) / glm::vec2(2)));

            glm::vec4 color = normalColor;
            std::shared_ptr<Graphics::Texture> image = normalImage;

            if(!active)
            {
                glm::vec4 inactiveColor(0.7f);
                if(image)
                    renderer.DrawTexture(*image, pos, size, inactiveColor);
                else
                    renderer.DrawRect(pos, size, inactiveColor);
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
                renderer.DrawTexture(*image, pos, size, color);
            else
                renderer.DrawRect(pos, size, color);
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
}
