#pragma once

#include <glm/glm.hpp>

namespace RIS
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() {};

        virtual void Clear(const glm::vec4 &clearColor) = 0;
        virtual void Present() = 0;
    };
}
