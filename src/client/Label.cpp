#include "Label.hpp"

#include <string>

using std::string;

namespace RIS
{
    UILabel::UILabel(const SystemLocator &systems)
        : Component(systems), fontColor(1, 1, 1, 1), isVisible(true), fontSize(-1), font(0)
    {
    }

    UILabel::~UILabel()
    {
    }

    void UILabel::SetFont(int font, float fontSize)
    {
        this->font = font;
        this->fontSize = fontSize;
    }

    void UILabel::SetTextColor(const glm::vec4 &color)
    {
        fontColor = color;
    }

    void UILabel::SetVisible(bool visible)
    {
        isVisible = visible;
    }

    void UILabel::SetPosition(const glm::vec2 &position)
    {
        this->position = position;
    }

    void UILabel::SetText(const string &text)
    {
        this->text = text;
    }

    void UILabel::Update()
    {

    }

    void UILabel::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        renderer.DrawText(text, font, parentPosition + position, fontSize, fontColor);
    }
}
