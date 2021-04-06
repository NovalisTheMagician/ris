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
    using ButtonFunc = std::function<void()>;

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

        void Draw(Graphics::SpriteRenderer &renderer);

        void OnMouseMove(float x, float y);
        void OnMouseDown(Input::InputKey button);
        void OnMouseUp(Input::InputKey button);

    private:
        std::string text = "";

        glm::vec4 textColor = glm::vec4(0, 0, 0, 1);

        glm::vec4 normalColor = glm::vec4(1, 1, 1, 1);
        glm::vec4 hoverColor = glm::vec4(0.7f, 0.7f, 0.7f, 1);
        glm::vec4 downColor = glm::vec4(0.5f, 0.5f, 0.5f, 1);
        Graphics::Texture::Ptr normalImage, hoverImage, downImage;

        bool isInBounds = false;
        bool isClickedDown = false;

        ButtonFunc callback;

        bool active = true;

    };
}
