#include "ui/Button.hpp"

#include "misc/Logger.hpp"

#include <iostream>

namespace RIS
{
    namespace UI
    {
        Button::Button()
            : callback([](){})
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

        void Button::SetFont(int font, float size)
        {
            this->font = font;
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

        void Button::SetImages(int normal, int hover, int down)
        {
            normalImage = normal;
            hoverImage = hover;
            downImage = down;
        }

        void Button::SetNormalColor(const glm::vec4 &color)
        {
            normalColor = color;
        }

        void Button::SetNormalImage(int image)
        {
            normalImage = image;
        }

        void Button::SetHoverColor(const glm::vec4 &color)
        {
            hoverColor = color;
        }

        void Button::SetHoverImage(int image)
        {
            hoverImage = image;
        }

        void Button::SetDownColor(const glm::vec4 &color)
        {
            downColor = color;
        }

        void Button::SetDownImage(int image)
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

        void Button::Draw(const glm::vec2 &parentPosition)
        {
            parentPos = parentPosition;

            glm::vec2 pos = parentPosition + position;
            //TextMetrics metrics = renderer.MeasureText(text, font, fontSize);
            //glm::vec2 textPos = pos + ((size / glm::vec2(2)) - (glm::vec2(metrics.width, metrics.height) / glm::vec2(2)));

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

            //renderer.SetTexture(image, 0);
            //renderer.DrawQuad(pos, size, color);
            //renderer.DrawText(text, font, textPos, fontSize, textColor);
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

        void Button::OnMouseDown(Input::InputButton button)
        {
            if(button == Input::InputButton::LEFT && isInBounds)
            {
                isClickedDown = true;
            }
        }

        void Button::OnMouseUp(Input::InputButton button)
        {
            if(button == Input::InputButton::LEFT && isClickedDown)
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
