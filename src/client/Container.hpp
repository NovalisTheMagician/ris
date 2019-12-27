#pragma once

#include "Component.hpp"

namespace RIS
{
    class Container : public Component
    {
    public:
        Container(const SystemLocator &systems) : Component(systems) {};
        virtual ~Container() = default;

        virtual void Add(ComponentPtr component) = 0;
        virtual void Remove(ComponentPtr component) = 0;
        virtual void RemoveAll() = 0;
        virtual ComponentPtr Find(const std::string &name, bool recursive = false) = 0;
    };
    using ContainerPtr = std::shared_ptr<Container>;
}
