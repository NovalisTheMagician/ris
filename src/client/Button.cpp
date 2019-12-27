#include "Button.hpp"

#include "common/Logger.hpp"

#include <iostream>

namespace RIS
{
    UIButton::UIButton(const SystemLocator &systems)
        : Component(systems), action(UIAction::Nop), text(""), font(1), fontSize(-1), normalColor(1, 1, 1, 1), hoverColor(0.7f, 0.7f, 0.7f, 1), 
            downColor(0.5f, 0.5f, 0.5f, 1), normalImage(1), hoverImage(1), downImage(1), textColor(0, 0, 0, 1), isInBounds(false),
            isClickedDown(false), param1(""), param2("")
    {
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

    void UIButton::SetAction(UIAction action, const std::string &param1, const std::string &param2)
    {
        this->action = action;
        this->param1 = param1;
        this->param2 = param2;
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

    void UIButton::OnMouseDown(int button)
    {
        if(button == 0 && isInBounds)
        {
            isClickedDown = true;
        }
    }

    void UIButton::OnMouseUp(int button)
    {
        auto &window = systems.GetWindow();

        if(button == 0 && isClickedDown)
        {
            if(isInBounds)
            {
                //do action
                std::cout << "Doing action " << std::to_string(static_cast<int>(action)) << std::endl;
                switch(action)
                {
                case UIAction::Quit: window.Exit(0); break;
                case UIAction::ChangeUI: 
                    {
                        if(!param1.empty())
                        {
                            systems.GetUserinterface().LoadLayout(param1);
                        }
                        else
                        {
                            Logger::Instance().Warning("Tried to change layout to empty");
                        }
                    }
                    break;
                case UIAction::SetValue:
                    {

                    }
                    break;
                }
            }
            isClickedDown = false;
        }
    }
}
