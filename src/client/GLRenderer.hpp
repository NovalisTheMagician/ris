#pragma once

#include "common/IRenderer.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <stdexcept>

#include <glbinding/gl46core/gl.h>
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

    struct Glyph
    {
        float advanceX;
        float bboxWidth, bboxHeight;
        float bearingX, bearingY;
        char charCode;
        float s0, s1, t0, t1;
        std::unordered_map<char, float> kernings;
    };

    struct Font
    {
        float ascender, descender;
        int bitmapWidth, bitmapHeight;
        float height;
        float maxAdvance;
        std::string name;
        float size;
        float spaceAdvance;
        std::unordered_map<char, Glyph> glyphs;

        int textureId;
    };

    class GLRenderer;

    class GL2DRenderer : public I2DRenderer
    {
    public:
        GL2DRenderer(GLRenderer &renderer);
        ~GL2DRenderer();

        void Setup();

        int LoadFont(const std::string &fontName) override;
        void DestroyFont(int fontId) override;

        void SetViewsize(int width, int height) override;

        void SetTexture(int textureId, int textureUnit) override;

        void Begin() override;
        void End() override;

        void DrawText(const std::string &text, int fontId, const glm::vec2 &position, float size, const glm::vec4 &color) override;
        void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) override;

        TextMetrics MeasureText(const std::string &text, int fontId, float size) override;
        float MaxHeightFont(int fontId, float size) override;

    private:
        static const int MAX_CHARS;

    private:
        struct PerFrameBuffer
        {
            glm::vec2 viewSize;
        };

        struct PerObjectBuffer
        {
            glm::vec4 color;
            glm::vec2 position;
            glm::vec2 size;
        };

    private:
        GLRenderer &renderer;

        PerFrameBuffer perFrame;
        PerObjectBuffer perObject;

        Buffer<PerFrameBuffer> perFrameBuffer;
        Buffer<PerObjectBuffer> perObjectBuffer;

        Buffer<VertexType::UIVertex> textBuffer, uiBuffer;
        VertexArray uiLayout;

        int highestUnusedFontId;
        std::unordered_map<int, Font> fonts;
        std::unordered_map<std::string, int> loadedFonts;

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

        void PostInit() override;

        int LoadTexture(const std::string &name, bool flip = true) override;
        void DestroyTexture(int texId) override;

        int CreateFramebuffer(int width = -1, int height = -1, bool useDepth = true) override;
        void DestroyFramebuffer(int framebufId) override;

        void SetFramebuffer(int framebufferId) override;
        void FramebufferResize(int framebufferId, int width, int height) override;

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
        std::unordered_map<std::string, int> loadedTextures;

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

        bool useAmdFix;

        friend GL2DRenderer;

    };
}
