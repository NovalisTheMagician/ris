#pragma once

#include "common/IRenderer.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <stdexcept>

#include <glbinding/gl46/gl.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>

#include "VertexTypes.hpp"

#include "Buffer.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Sampler.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "ProgramPipeline.hpp"

namespace RIS
{
    struct RendererException : public std::runtime_error
	{
		RendererException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class Camera
    {
    public:
        Camera();
        ~Camera();

    private:
        glm::mat4 projection;
        glm::mat4 view;

        glm::vec3 position;
        float yaw, pitch;

    };

    class GLRenderer;

    class GL2DRenderer : public I2DRenderer
    {
    public:
        GL2DRenderer(GLRenderer &renderer);
        ~GL2DRenderer();

        void Setup();

        void SetViewsize(int width, int height) override;
        void SetPosition(const glm::vec2 &position) override;

        void SetTexture(int textureId, int textureUnit) override;
        void SetColor(const glm::vec4 &color) override;

        void Begin() override;
        void End() override; // ??? really needed?

        void DrawText(const std::string &text) override;
        void DrawQuad(int width, int height) override;

    private:
        static const int MAX_CHARS;

    private:
        struct PerFrameBuffer
        {
            glm::mat4 projection;
        };

        struct PerObjectBuffer
        {
            glm::mat4 world;
            glm::vec4 color;
        };

    private:
        GLRenderer &renderer;

        PerFrameBuffer perFrame;
        PerObjectBuffer perObject;

        Buffer<PerFrameBuffer> perFrameBuffer;
        Buffer<PerObjectBuffer> perObjectBuffer;

        Buffer<VertexType::UIVertex> textBuffer, uiBuffer;
        VertexArray uiLayout;

    };

    class GLRenderer : public IRenderer
    {
    public:
        GLRenderer(const SystemLocator &systems, Config &config);
        ~GLRenderer();

        GLRenderer(const GLRenderer &) = delete;
        GLRenderer(GLRenderer &&) = delete;
        GLRenderer &operator=(const GLRenderer &) = delete;
        GLRenderer &operator=(GLRenderer &&) = delete;

        void LoadRequiredResources() override;

        int LoadTexture(const std::string &name) override;
        void DestroyTexture(int texId) override;

        int CreateFramebuffer(int width = -1, int height = -1, bool useDepth = true) override;
        void DestroyFramebuffer(int framebufId) override;

        void SetFramebuffer(int framebufferId) override;

        void Clear(int framebufferId, const glm::vec4 &clearColor = glm::vec4(0, 0, 0, 1), float depth = 1.0f) override;

        void Draw(int framebufferId) override;

        void Resize(int width, int height) override;

        I2DRenderer& Get2DRenderer() override;

    public:
        static const int DEFAULT_FRAMBUFFER_ID;
        static const int DEFAULT_TEXTURE_ID;
        static const int MISSING_TEXTURE_ID;

    private:
        const SystemLocator &systems;
        Config &config;

        int highestUnusedTexId;
        std::unordered_map<int, Texture> textures;

        int highestUnusedFrambufId;
        std::unordered_map<int, Framebuffer> framebuffers;

        Buffer<VertexType::UIVertex> fullscreenQuad;
        VertexArray postprocessVAO;
        Shader ppVertex, ppCopy;

        Sampler defaultSampler, uiSampler;

        ProgramPipeline pipeline;
        Shader uiVertex, uiFragment, uiText;

        GL2DRenderer renderer2d;
        Camera camera;

        friend GL2DRenderer;

    };
}
