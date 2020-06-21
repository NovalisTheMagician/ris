#include "ui/Label.hpp"

#include <string>

using std::string;

namespace RIS
{
    namespace UI
    {
        Label::Ptr Label::Create(std::shared_ptr<Graphics::Font> defaultFont)
        {
            return std::make_shared<Label>(defaultFont);
        }

        Label::Label(std::shared_ptr<Graphics::Font> defaultFont)
            : font(defaultFont)
        {}

        void Label::SetFont(std::shared_ptr<Graphics::Font> font)
        {
            this->font = font;
        }

        void Label::SetFontSize(float fontSize)
        {
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
