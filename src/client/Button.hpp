#pragma once

#include "Component.hpp"

#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include <functional>

namespace RIS
{
    using ButtonFunc = std::function<void()>;

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

        void SetCallback(ButtonFunc func);

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

        void OnMouseMove(float x, float y) override;
        void OnMouseDown(InputButton button) override;
        void OnMouseUp(InputButton button) override;

    private:
        std::string text = "";

        int font = 1;
        float fontSize = -1;
        glm::vec4 textColor = glm::vec4(0, 0, 0, 1);

        glm::vec2 position, size;

        glm::vec4 normalColor = glm::vec4(1, 1, 1, 1);
        glm::vec4 hoverColor = glm::vec4(0.7f, 0.7f, 0.7f, 1);
        glm::vec4 downColor = glm::vec4(0.5f, 0.5f, 0.5f, 1);
        int normalImage = 1, hoverImage = 1, downImage = 1;

        glm::vec2 parentPos;

        bool isInBounds = false;
        bool isClickedDown = false;

        ButtonFunc callback;

    };
    using ButtonPtr = std::shared_ptr<UIButton>;
}

#define MakeButton(x) std::make_shared<UIButton>(x)
