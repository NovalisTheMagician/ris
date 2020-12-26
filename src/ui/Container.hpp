#pragma once

#include "ui/Component.hpp"

namespace RIS::UI
{
    class Container : public Component
    {
    public:
        using Ptr = std::shared_ptr<Container>;

        virtual ~Container() = default;

        virtual void Add(Component::Ptr component) = 0;
        virtual void Remove(Component::Ptr component) = 0;
        virtual void RemoveAll() = 0;
        virtual Component::Ptr Find(const std::string &name, bool recursive = false) = 0;
    };
}
