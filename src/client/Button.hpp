#pragma once

#include "Component.hpp"

#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include "UIAction.hpp"

namespace RIS
{
    class UIButton : public Component
    {
    public:
        UIButton(const SystemLocator &systems);
        virtual ~UIButton();

        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);

        void SetText(const std::string &text);
        void SetFont(int font, float size);
        void SetTextColor(const glm::vec4 &color);

        void SetColors(const glm::vec4 &normal, const glm::vec4 &hover, const glm::vec4 &down);
        void SetImages(int normal, int hover, int down);

        void SetNormalColor(const glm::vec4 &color);
        void SetNormalImage(int image);
        void SetHoverColor(const glm::vec4 &color);
        void SetHoverImage(int image);
        void SetDownColor(const glm::vec4 &color);
        void SetDownImage(int image);

        void SetAction(UIAction action, const std::string &param1 = "", const std::string &param2 = "");

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

        void OnMouseMove(float x, float y) override;
        void OnMouseDown(InputButtons button) override;
        void OnMouseUp(InputButtons button) override;

    private:
        std::string text;

        int font;
        float fontSize;
        glm::vec4 textColor;

        glm::vec2 position, size;

        glm::vec4 normalColor, hoverColor, downColor;
        int normalImage, hoverImage, downImage;

        UIAction action;
        std::string param1, param2;

        glm::vec2 parentPos;

        bool isInBounds;
        bool isClickedDown;

    };
    using ButtonPtr = std::shared_ptr<UIButton>;
}

#define MakeButton(x) std::make_shared<UIButton>(x)
