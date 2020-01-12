#pragma once

#include <glm/glm.hpp>
#include <string>

namespace RIS
{
    struct TextMetrics
    {
        float width;
        float height;
    };

    class I2DRenderer
    {
    public:
        virtual ~I2DRenderer() = default;

        virtual void SetViewsize(int width, int height) = 0;

        virtual int LoadFont(const std::string &fontName) = 0;
        virtual void DestroyFont(int fontId) = 0;

        virtual void SetTexture(int textureId, int textureUnit) = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void DrawText(const std::string &text, int fontId, const glm::vec2 &position, float size, const glm::vec4 &color) = 0;
        virtual void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) = 0;

        virtual TextMetrics MeasureText(const std::string &text, int fontId, float size) = 0;
        virtual float MaxHeightFont(int fontId, float size) = 0;
    };

    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void LoadRequiredResources() = 0;

        virtual int LoadTexture(const std::string &name, bool flip = true) = 0;
        virtual void DestroyTexture(int texId) = 0;

        virtual int CreateFramebuffer(int width = -1, int height = -1, bool useDepth = true) = 0;
        virtual void DestroyFramebuffer(int framebufId) = 0;

        virtual void SetFramebuffer(int framebufferId) = 0;
        virtual void FramebufferResize(int framebufferId, int width, int height) = 0;

        virtual void Clear(int framebufferId, const glm::vec4 &clearColor = glm::vec4(0, 0, 0, 1), float depth = 1.0f) = 0;

        virtual void Draw(int framebufferId) = 0;

        virtual void Resize(int width, int height) = 0;

        virtual I2DRenderer& Get2DRenderer() = 0;
    };
}
