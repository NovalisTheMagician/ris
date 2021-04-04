#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "misc/Timer.hpp"

#include "input/KeyDefs.hpp"

#include "graphics/SpriteRenderer.hpp"
#include "graphics/Framebuffer.hpp"
#include "graphics/Font.hpp"

namespace RIS::UI
{
    enum class Anchor
    {
        Top,
        Left,
        Bottom,
        Right,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    class Component
    {
    public:
        Component(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont) : parentFramebuffer(parentFramebuffer), font(defaultFont) {};
        virtual ~Component() = default;

        virtual void OnMouseMove(float x, float y) {};
        virtual void OnMouseDown(Input::InputKey mouseCode) {};
        virtual void OnMouseUp(Input::InputKey mouseCode) {};
        virtual void OnMouseWheel(float x, float y) {};

        virtual void OnKeyDown(Input::InputKey keyCode) {};
        virtual void OnKeyUp(Input::InputKey keyCode) {};
        virtual void OnKeyRepeat(Input::InputKey keyCode) {};

        virtual void OnChar(uint32_t c) {};

        virtual Component& SetName(const std::string &name) { this->name = name; return *this; };
        virtual std::string GetName() const { return name; };

        virtual Component& SetAnchor(Anchor anchor) { this->anchor = anchor; return *this; };
        virtual Anchor GetAnchor() const { return anchor; };

        virtual Component& SetPosition(const glm::vec2 &position) { this->position = position; return *this; };
        virtual Component& SetSize(const glm::vec2 &size) { this->size = size; return *this; };

        virtual void Update(const Timer &timer) = 0;
        virtual void Draw(Graphics::SpriteRenderer &renderer) = 0;

    protected:
        std::string name;
        glm::vec2 position;
        glm::vec2 size = glm::vec2(64, 32);
        Anchor anchor = Anchor::TopLeft;
        Graphics::Framebuffer &parentFramebuffer;
        Graphics::Font::Ptr font;

    };
}
