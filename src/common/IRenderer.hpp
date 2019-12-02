#pragma once

#include <glm/glm.hpp>

namespace RIS
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void LoadRequiredResources() = 0;

        virtual void Clear(const glm::vec4 &clearColor) = 0;
        virtual void Resize(int width, int height) = 0;
    };
}
