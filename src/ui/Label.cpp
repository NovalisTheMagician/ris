#include "ui/Label.hpp"

#include <string>

using std::string;

namespace RIS
{
    namespace UI
    {
        Label::Label(std::shared_ptr<Graphics::Font> defaultFont)
            : font(defaultFont)
        {}

        void Label::SetFont(std::shared_ptr<Graphics::Font> font, float fontSize)
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

        void Label::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
        {
            renderer.DrawString(text, *font.get(), fontSize, parentPosition + position, fontColor);
        }
    }
}
