#pragma once

#include <glm/glm.hpp>
#include <string>

namespace RIS
{
    class I2DRenderer
    {
    public:
        virtual ~I2DRenderer();

        virtual void SetViewsize(int width, int height) = 0;
        virtual void SetPosition(const glm::vec2 &positino) = 0;

        virtual void SetTexture(int textureId, int textureUnit) = 0;

        virtual void Begin() = 0;
        virtual void End() = 0; // ??? really needed?

        virtual void DrawText(const std::string &text) = 0;
        virtual void DrawQuad(int width, int height) = 0;
    };

    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void LoadRequiredResources() = 0;

        virtual int LoadTexture(const std::string &name) = 0;
        virtual void DestroyTexture(int texId) = 0;

        virtual int CreateFramebuffer(int width = -1, int height = -1, bool useDepth = true) = 0;
        virtual void DestroyFramebuffer(int framebufId) = 0;

        virtual void SetFramebuffer(int framebufferId) = 0;

        virtual void Clear(int framebufferId, const glm::vec4 &clearColor) = 0;

        virtual void Resize(int width, int height) = 0;

        virtual I2DRenderer& Get2DRenderer() const = 0;
    };
}
