#pragma once

#include <memory>
#include <string>

#include "common/SystemLocator.hpp"

namespace RIS
{
    class Component
    {
    public:
        Component(const SystemLocator &systems) : systems(systems) {};
        virtual ~Component() = default;

        virtual void OnMouseMove(float x, float y) {};
        virtual void OnMouseDown(int mouseCode) {};
        virtual void OnMouseUp(int mouseCode) {};

        virtual void OnKeyDown(int keyCode) {};
        virtual void OnKeyUp(int keyCode) {};

        virtual void OnChar(char c) {};

        virtual void SetName(const std::string &name) { this->name = name; };
        virtual std::string GetName() const { return name; };

        virtual void Update() = 0;
        virtual void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) = 0;

    protected:
        const SystemLocator &systems;

        std::string name;

    };
    using ComponentPtr = std::shared_ptr<Component>;
}
