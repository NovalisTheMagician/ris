#pragma once

#include <glm/glm.hpp>
#include <string>

namespace RIS
{
    using ResourceId = uint32_t;

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

        virtual ResourceId LoadFont(const std::string &fontName) = 0;
        virtual void DestroyFont(ResourceId fontId) = 0;

        virtual void SetTexture(ResourceId textureId, int textureUnit) = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void DrawText(const std::string &text, ResourceId fontId, const glm::vec2 &position, float size, const glm::vec4 &color) = 0;
        virtual void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) = 0;

        virtual TextMetrics MeasureText(const std::string &text, ResourceId fontId, float size) = 0;
        virtual float MaxHeightFont(ResourceId fontId, float size) = 0;
    };

    class I3DRenderer
    {
    public:
        virtual ~I3DRenderer() = default;

        virtual ResourceId LoadMesh(const std::string &name) = 0;
        virtual void DestroyMesh(ResourceId meshId) = 0;

        virtual ResourceId LoadModel(const std::string &name) = 0;
        virtual void DestroyModel(ResourceId modelId) = 0;

        virtual void SetTexture(ResourceId textureId) = 0;

        virtual void Begin(const glm::mat4 &viewProjection) = 0;
        virtual void End() = 0;

        virtual void Draw(ResourceId modelId, const glm::mat4 &world) = 0;
    };

    class IPostProcessor
    {
    public:
        virtual ~IPostProcessor() = default;

        virtual void SetFramebufferTexture(ResourceId framebufId, uint32_t place) = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Draw(int pp) = 0;

    };

    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void PostInit() = 0;

        virtual ResourceId LoadTexture(const std::string &name, bool flip = false) = 0;
        virtual void DestroyTexture(ResourceId texId) = 0;

        virtual ResourceId CreateFramebuffer(int width = -1, int height = -1, bool useDepth = true) = 0;
        virtual void DestroyFramebuffer(ResourceId framebufId) = 0;

        virtual void SetFramebuffer(ResourceId framebufferId) = 0;
        virtual void FramebufferResize(ResourceId framebufferId, int width, int height) = 0;

        virtual void Clear(ResourceId framebufferId, const glm::vec4 &clearColor = glm::vec4(0, 0, 0, 1), float depth = 1.0f) = 0;

        virtual void Resize(int width, int height) = 0;

        virtual I2DRenderer& Get2DRenderer() = 0;
        virtual I3DRenderer& Get3DRenderer() = 0;
        virtual IPostProcessor& GetPostProcessor() = 0;
    };
}
