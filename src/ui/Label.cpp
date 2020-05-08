#include "ui/Label.hpp"

#include <string>

using std::string;

namespace RIS
{
    namespace UI
    {
        void Label::SetFont(int font, float fontSize)
        {
            this->font = font;
            this->fontSize = fontSize;
        }

        void Label::SetTextColor(const glm::vec4 &color)
        {
            fontColor = color;
        }

        void Label::SetVisible(bool visible)
        {
            isVisible = visible;
        }

        void Label::SetPosition(const glm::vec2 &position)
        {
            this->position = position;
        }

        void Label::SetText(const string &text)
        {
            this->text = text;
        }

        void Label::Update()
        {

        }

        void Label::Draw(const glm::vec2 &parentPosition)
        {
            //renderer.DrawText(text, font, parentPosition + position, fontSize, fontColor);
        }
    }
}
