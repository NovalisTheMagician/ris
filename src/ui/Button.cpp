#include "ui/Button.hpp"

#include "misc/Logger.hpp"

#include <iostream>

namespace RIS
{
    namespace UI
    {
        Button::Button(std::shared_ptr<Graphics::Font> defaultFont)
            : callback([](){}), font(defaultFont)
        {
        }

        void Button::SetPosition(const glm::vec2 &position)
        {
            this->position = position;
        }

        void Button::SetSize(const glm::vec2 &size)
        {
            this->size = size;
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

        void Button::Update()
        {
            
        }

        void Button::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
        {
            parentPos = parentPosition;

            glm::vec2 pos = parentPosition + position;
            Graphics::TextMetrics metrics = font->MeasureString(text, fontSize);
            glm::vec2 textPos = pos + ((size / glm::vec2(2)) - (glm::vec2(metrics.width, metrics.height) / glm::vec2(2)));

            glm::vec4 color = normalColor;
            std::shared_ptr<Graphics::Texture> image = normalImage;

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
            glm::vec2 pos = parentPos + position;

            if( x > pos.x && x < pos.x + size.x &&
                y > pos.y && y < pos.y + size.y)
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
                if(isInBounds)
                {
                    callback();
                }
                isClickedDown = false;
            }
        }
    }
}
