#pragma once

#include "ui/Component.hpp"

#include "graphics/Font.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS::UI
{
    class Label
    {
    public:
        Label(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont);
        virtual ~Label() = default;
        Label(const Label&) = delete;
        Label& operator=(const Label&) = delete;
        Label(Label&&) = default;
        Label& operator=(Label&&) = default;

        Label& SetName(const std::string &name);
        std::string GetName() const;
        Label& SetAnchor(Anchor anchor);
        Anchor GetAnchor() const;
        Label& SetPosition(const glm::vec2 &position);
        Label& SetSize(const glm::vec2 &size);
        Label& SetFont(std::shared_ptr<Graphics::Font> font);
        Label& SetFontSize(float fontSize);
        Label& SetTextColor(const glm::vec4 &color);
        Label& SetVisible(bool visible);
        Label& SetText(const std::string &text);

        void Update(const Timer &timer);
        void Draw(Graphics::SpriteRenderer &renderer);

        void OnMouseMove(float x, float y) {};
        void OnMouseDown(Input::InputKey mouseCode) {};
        void OnMouseUp(Input::InputKey mouseCode) {};
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
        glm::vec4 fontColor = glm::vec4(1, 1, 1, 1);
        bool isVisible = true;

    };
}
