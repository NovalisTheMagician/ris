#pragma once

#include "ui/Component.hpp"

#include "graphics/Font.hpp"
#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include <functional>

namespace RIS::UI
{
    class Button;

    using ButtonFunc = std::function<void(Button&)>;

    class Button : public Component<Button>
    {
    public:
        Button(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize);
        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;
        Button(Button&&) = default;
        Button& operator=(Button&&) = default;

        Button& SetText(const std::string &text);
        std::string GetText() const;
        Button& SetTextColor(const glm::vec4 &color);
        Button& SetCallback(ButtonFunc func);
        Button& SetActive(bool isActive);
        Button& SetToggleMode(bool isToggle);
        Button& SetToggle(bool toggleOn);
        bool IsToggle() const;
        Button& SetNormalTexture(Graphics::Texture::Ptr normalTexture);
        Button& SetHoverTexture(Graphics::Texture::Ptr hoverTexture);
        Button& SetDownTexture(Graphics::Texture::Ptr downTexture);

        void Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset);

        void OnMouseMove(float x, float y);
        void OnMouseDown(Input::InputKey button);
        void OnMouseUp(Input::InputKey button);

    private:
        std::string text = "";

        bool isToggle = false;
        bool toggleOn = false;

        glm::vec4 textColor = glm::vec4(0, 0, 0, 1);

        glm::vec4 normalColor = glm::vec4(1, 1, 1, 1);
        glm::vec4 hoverColor = glm::vec4(0.7f, 0.7f, 0.7f, 1);
        glm::vec4 downColor = glm::vec4(0.5f, 0.5f, 0.5f, 1);
        Graphics::Texture::Ptr normalImage, hoverImage, downImage, inactiveImage;

        bool isInBounds = false;
        bool isClickedDown = false;

        ButtonFunc callback;

        bool active = true;

    };
}
