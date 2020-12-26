#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "misc/Timer.hpp"

#include "input/KeyDefs.hpp"

#include "graphics/SpriteRenderer.hpp"

namespace RIS::UI
{
    class Component
    {
    public:
        using Ptr = std::shared_ptr<Component>;

        virtual ~Component() = default;

        virtual void OnMouseMove(float x, float y) {};
        virtual void OnMouseDown(Input::InputKey mouseCode) {};
        virtual void OnMouseUp(Input::InputKey mouseCode) {};
        virtual void OnMouseWheel(float x, float y) {};

        virtual void OnKeyDown(Input::InputKey keyCode) {};
        virtual void OnKeyUp(Input::InputKey keyCode) {};
        virtual void OnKeyRepeat(Input::InputKey keyCode) {};

        virtual void OnChar(uint32_t c) {};

        virtual void SetName(const std::string &name) { this->name = name; };
        virtual std::string GetName() const { return name; };

        virtual void SetPosition(const glm::vec2 &position) { this->position = position; };
        virtual void SetSize(const glm::vec2 &size) { this->size = size; };

        virtual void Update(const Timer &timer) = 0;
        virtual void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) = 0;

    protected:
        std::string name;
        glm::vec2 position;
        glm::vec2 size;

    };
}
