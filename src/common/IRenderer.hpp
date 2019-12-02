#pragma once

#include <glm/glm.hpp>
#include <string>

namespace RIS
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void LoadRequiredResources() = 0;

        virtual int LoadTexture(const std::string &name) = 0;
        virtual void DestroyTexture(int texId) = 0;

        virtual void Clear(const glm::vec4 &clearColor) = 0;
        virtual void Resize(int width, int height) = 0;
    };
}
