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

    class Button
    {
    public:
        Button(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont);
        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;
        Button(Button&&) = default;
        Button& operator=(Button&&) = default;

        Button& SetName(const std::string &name);
        std::string GetName() const;
        Button& SetAnchor(Anchor anchor);
        Anchor GetAnchor() const;
        Button& SetPosition(const glm::vec2 &position);
        Button& SetSize(const glm::vec2 &size);
        Button& SetText(const std::string &text);
        Button& SetFont(std::shared_ptr<Graphics::Font> font);
        Button& SetFontSize(float size);
        Button& SetTextColor(const glm::vec4 &color);
        Button& SetCallback(ButtonFunc func);
        Button& SetActive(bool isActive);

        void Update(const Timer &timer);
        void Draw(Graphics::SpriteRenderer &renderer);

        void OnMouseMove(float x, float y);
        void OnMouseDown(Input::InputKey button);
        void OnMouseUp(Input::InputKey button);

        void OnMouseWheel(float x, float y) {};

        void OnKeyDown(Input::InputKey keyCode) {};
        void OnKeyUp(Input::InputKey keyCode) {};
        void OnKeyRepeat(Input::InputKey keyCode) {};

        void OnChar(uint32_t c) {};

    private:
        std::string name;
        glm::vec2 position;
        glm::vec2 size = glm::vec2(64, 32);
        Anchor anchor = Anchor::TopLeft;
        Graphics::Framebuffer &parentFramebuffer;
        Graphics::Font::Ptr font;

        std::string text = "";

        float fontSize = -1;
        glm::vec4 textColor = glm::vec4(0, 0, 0, 1);

        glm::vec4 normalColor = glm::vec4(1, 1, 1, 1);
        glm::vec4 hoverColor = glm::vec4(0.7f, 0.7f, 0.7f, 1);
        glm::vec4 downColor = glm::vec4(0.5f, 0.5f, 0.5f, 1);
        std::shared_ptr<Graphics::Texture> normalImage, hoverImage, downImage;

        bool isInBounds = false;
        bool isClickedDown = false;

        ButtonFunc callback;

        bool active = true;

    };
}
