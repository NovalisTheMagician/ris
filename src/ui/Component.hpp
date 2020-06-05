#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "input/KeyDefs.hpp"

#include "graphics/SpriteRenderer.hpp"

namespace RIS
{
    namespace UI
    {
        class Component
        {
        public:
            virtual ~Component() = default;

            virtual void OnMouseMove(float x, float y) {};
            virtual void OnMouseDown(Input::InputButton mouseCode) {};
            virtual void OnMouseUp(Input::InputButton mouseCode) {};
            virtual void OnMouseWheel(float x, float y) {};

            virtual void OnKeyDown(Input::InputKey keyCode) {};
            virtual void OnKeyUp(Input::InputKey keyCode) {};

            virtual void OnChar(char c) {};

            virtual void SetName(const std::string &name) { this->name = name; };
            virtual std::string GetName() const { return name; };

            virtual void Update() = 0;
            virtual void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) = 0;

        protected:
            std::string name;

        };
        using ComponentPtr = std::shared_ptr<Component>;
    }
}
