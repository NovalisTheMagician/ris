#pragma once

#include <string>

#include <glm/glm.hpp>

#include "misc/Timer.hpp"

#include "input/KeyDefs.hpp"

#include "graphics/SpriteRenderer.hpp"
#include "graphics/Framebuffer.hpp"
#include "graphics/Font.hpp"

#include <algorithm>

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

    template<typename Collection, typename Func>
    void ForeachDispatch(Collection &c, Func func)
    {
        std::for_each(std::begin(c), std::end(c), [func](auto &v){ std::visit(func, v); });
    }

    template<typename T>
    struct Component
    {
        Component(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, glm::vec2 parentSize) : parentFramebuffer(parentFramebuffer), font(defaultFont), parentSize(parentSize) {}
        T& SetName(const std::string &name) { this->name = name; return *static_cast<T*>(this); }
        std::string GetName() const { return name; }
        T& SetAnchor(Anchor anchor) { this->anchor = anchor; return *static_cast<T*>(this); }
        Anchor GetAnchor() const { return anchor; }
        T& SetPosition(const glm::vec2 &position) { this->position = position; return *static_cast<T*>(this); }
        glm::vec2 GetPosition() const { return position; }
        T& SetSize(const glm::vec2 &size) { this->size = size; return *static_cast<T*>(this); }
        glm::vec2 GetSize() const { return size; }
        T& SetFont(Graphics::Font::Ptr font) { this->font = font; return *static_cast<T*>(this); }
        Graphics::Font::Ptr GetFont() const { return font; }
        T& SetFontSize(float fontSize) { this->fontSize = fontSize; return *static_cast<T*>(this); }
        float GetFontSize() const { return fontSize; }

        void Update(const Timer &timer) {}

        void OnMouseMove(float x, float y) {}
        void OnMouseDown(Input::InputKey button) {}
        void OnMouseUp(Input::InputKey button) {}
        void OnMouseWheel(float x, float y) {}
        void OnKeyDown(Input::InputKey keyCode) {}
        void OnKeyUp(Input::InputKey keyCode) {}
        void OnKeyRepeat(Input::InputKey keyCode) {}
        void OnChar(uint32_t c) {}

    protected:
        std::string name;
        glm::vec2 position;
        glm::vec2 size = glm::vec2(64, 32);
        Anchor anchor = Anchor::TopLeft;
        Graphics::Framebuffer &parentFramebuffer;
        Graphics::Font::Ptr font;
        float fontSize = 16.0f;
        glm::vec2 parentSize;

    };
}
