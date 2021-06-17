#pragma once

#include <string>

#include <glm/glm.hpp>

#include "misc/Timer.hpp"

#include "input/KeyDefs.hpp"

#include "graphics/SpriteRenderer.hpp"
#include "graphics/Framebuffer.hpp"
#include "graphics/Font.hpp"

#include <algorithm>

#include <any>

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
        BottomRight,
        Center,
        CenterX,
        CenterY
    };

    template<typename Collection, typename Func>
    void ForeachDispatch(Collection &c, Func func)
    {
        std::for_each(std::begin(c), std::end(c), [func](auto &v){ std::visit(func, v); });
    }

    template<typename T>
    struct Component
    {
        Component(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, glm::vec2 parentSize) : parentFramebuffer(std::ref(parentFramebuffer)), font(defaultFont), parentSize(parentSize) {}
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
        T& SetScale(float scale) { this->scale = scale; return *static_cast<T*>(this); }
        float GetScale() const { return scale; }
        T& SetOffset(const glm::vec2 offset) { this->offset = offset; this->offset = glm::clamp(this->offset, -maxOffset, {0.0f, 0.0f}); return *static_cast<T*>(this); }
        glm::vec2 GetOffset() const { return offset; }
        T& SetMaxOffset(const glm::vec2 maxOffset) { this->maxOffset = maxOffset; return *static_cast<T*>(this); }
        glm::vec2 GetMaxOffset() const { return maxOffset; }
        T& SetOffsetStep(const glm::vec2 offsetStep) { this->offsetStep = offsetStep; return *static_cast<T*>(this); }
        glm::vec2 GetOffsetStep() const { return offsetStep; }
        T& UseMouseScrolling(bool useMousewheel) { this->useMousewheelForScrolling = useMousewheel; return *static_cast<T*>(this); }
        bool IsUsingMouseScrolling() const { return useMousewheelForScrolling; }
        T& SetVisible(bool visible) { this->visible = visible; return *static_cast<T*>(this); }
        bool IsVisible() const { return visible; }
        T& SetData(std::any data) { this->data = data; return *static_cast<T*>(this); }
        std::any GetData() const { return data; }
        T& SetActive(bool active) { this->active = active; return *static_cast<T*>(this); }
        bool IsActive() const { return active; }

        void Reset() { SetOffset({0, 0}); };

        void Update(const Timer &timer) {}

        void OnMouseMove(float x, float y) {}
        void OnMouseDown(Input::InputKey button) {}
        void OnMouseUp(Input::InputKey button) {}
        void OnMouseWheel(float x, float y) 
        { 
            if(!visible) return;
            if(useMousewheelForScrolling) 
            {
                offset += offsetStep * glm::vec2(x, y);
                offset = glm::clamp(offset, -maxOffset, {0.0f, 0.0f});
            }
        }
        void OnKeyDown(Input::InputKey keyCode) {}
        void OnKeyUp(Input::InputKey keyCode) {}
        void OnKeyRepeat(Input::InputKey keyCode) {}
        void OnChar(uint32_t c) {}

    protected:
        glm::vec2 GetAnchoredPosition() const
        {
            glm::vec2 pos;
            glm::vec2 ps = parentSize;
            glm::vec2 s = size;
            switch(anchor)
            {
            case Anchor::Right:
            case Anchor::TopRight:
                pos.x = ps.x - position.x - s.x;
                break;
            case Anchor::Bottom:
            case Anchor::BottomLeft:
                pos.y = ps.y - position.y - s.y;
                break;
            case Anchor::BottomRight:
                pos = ps - position - s;
                break;
            case Anchor::CenterX:
                pos.x = (ps.x / 2) - (s.x / 2) + position.x;
                break;
            case Anchor::CenterY:
                pos.y = (ps.y / 2) - (s.y / 2) + position.y;
                break;
            case Anchor::Center:
                pos = (ps / 2.0f) - (s / 2.0f) + position;
                break;
            case Anchor::Top:
            case Anchor::TopLeft:
            default:
                pos = position;
            }
            return pos;
        }

    protected:
        std::string name;
        glm::vec2 position = glm::vec2(0, 0);
        glm::vec2 size = glm::vec2(64, 32);
        float scale = 1.0f;
        bool visible = true;
        bool active = true;
        glm::vec2 offset = glm::vec2(0, 0);
        glm::vec2 maxOffset = glm::vec2(0, 0);
        glm::vec2 offsetStep = glm::vec2(0, 0);
        bool useMousewheelForScrolling = false;
        Anchor anchor = Anchor::TopLeft;
        std::reference_wrapper<Graphics::Framebuffer> parentFramebuffer;
        Graphics::Font::Ptr font;
        float fontSize = 16.0f;
        glm::vec2 parentSize;
        std::any data;

    };
}
