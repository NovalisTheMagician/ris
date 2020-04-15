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

#include "Mesh.hpp"

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

        ResourceId textureId;
    };

    class GLRenderer;

    class GL2DRenderer : public I2DRenderer
    {
    public:
        GL2DRenderer(GLRenderer &renderer);
        ~GL2DRenderer();

        void Setup();
        void LoadShaders();

        ResourceId LoadFont(const std::string &fontName) override;
        void DestroyFont(ResourceId fontId) override;

        void SetViewsize(int width, int height) override;

        void SetTexture(ResourceId textureId, int textureUnit) override;

        void Begin() override;
        void End() override;

        void DrawText(const std::string &text, ResourceId fontId, const glm::vec2 &position, float size, const glm::vec4 &color) override;
        void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) override;

        TextMetrics MeasureText(const std::string &text, ResourceId fontId, float size) override;
        float MaxHeightFont(ResourceId fontId, float size) override;

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

        ResourceId highestUnusedFontId;
        std::unordered_map<ResourceId, Font> fonts;
        std::unordered_map<std::string, ResourceId> loadedFonts;

        Shader uiVertex, uiFragment, uiText;
        Sampler uiSampler;

    };

    class GL3DRenderer : public I3DRenderer
    {
    public:
        GL3DRenderer(GLRenderer &renderer);
        ~GL3DRenderer();

        void Setup();
        void LoadShaders();

        ResourceId LoadMesh(const std::string &name) override;
        void DestroyMesh(ResourceId modelId) override;

        void Begin(const glm::mat4 &viewProjection) override;
        void End() override;

        void Draw(ResourceId modelId, const glm::mat4 &world) override;

    public:
        static const ResourceId MISSING_MODEL_ID;

    private:
        struct PerFrameMatrices
        {
            glm::mat4 viewProjection;
        };

        struct PerObjectMatrices
        {
            glm::mat4 world;
        };

    private:
        GLRenderer &renderer;

        ResourceId highestUnusedModelId = 1;
        std::unordered_map<ResourceId, Mesh> models;
        std::unordered_map<std::string, ResourceId> loadedModels;
        std::vector<Buffer<VertexType::ModelVertex>> vertexBuffers;
        std::vector<Buffer<uint16_t>> indexBuffers;

        VertexArray modelVAO;
        Shader staticModelShader, modelUnlitShader;
        Buffer<PerFrameMatrices> perFrameBuffer;
        Buffer<PerObjectMatrices> perObjectBuffer;
        PerFrameMatrices perFrameData;
        PerObjectMatrices perObjectData;

        Sampler defaultSampler;

    };

    class GLPostProcessor : public IPostProcessor
    {
    public:
        GLPostProcessor(GLRenderer &renderer);
        ~GLPostProcessor();

        void Setup();
        void LoadShaders();

        void SetFramebufferTexture(ResourceId framebufId, uint32_t place) override;

        void Begin() override;
        void End() override;

        void Draw(int pp) override;

    private:
        GLRenderer &renderer;

        Buffer<VertexType::UIVertex> fullscreenQuad;
        VertexArray postprocessVAO;
        Shader ppVertex, ppCopy;

        Sampler sampler;
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

        ResourceId LoadTexture(const std::string &name, bool flip = true) override;
        void DestroyTexture(ResourceId texId) override;

        ResourceId CreateFramebuffer(int width = -1, int height = -1, bool useDepth = true) override;
        void DestroyFramebuffer(ResourceId framebufId) override;

        void SetFramebuffer(ResourceId framebufferId) override;
        void FramebufferResize(ResourceId framebufferId, int width, int height) override;

        void Clear(ResourceId framebufferId, const glm::vec4 &clearColor = glm::vec4(0, 0, 0, 1), float depth = 1.0f) override;

        void Resize(int width, int height) override;

        I2DRenderer& Get2DRenderer() override;
        I3DRenderer& Get3DRenderer() override;
        IPostProcessor& GetPostProcessor() override;

    public:
        static const ResourceId DEFAULT_FRAMBUFFER_ID;
        static const ResourceId DEFAULT_TEXTURE_ID;
        static const ResourceId MISSING_TEXTURE_ID;

    private:
        const SystemLocator &systems;
        Config &config;

        ResourceId highestUnusedTexId = 2;
        std::unordered_map<ResourceId, Texture> textures;
        std::unordered_map<std::string, ResourceId> loadedTextures;

        ResourceId highestUnusedFrambufId = 1;
        std::unordered_map<ResourceId, Framebuffer> framebuffers;

        ProgramPipeline pipeline;

        GL2DRenderer renderer2d;
        GL3DRenderer renderer3d;
        GLPostProcessor postprocessor;

        bool useAmdFix = false;

        friend GL2DRenderer;
        friend GL3DRenderer;
        friend GLPostProcessor;

    };
}
