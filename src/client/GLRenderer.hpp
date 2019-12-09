#pragma once

#include "common/IRenderer.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <stdexcept>

#include <glbinding/gl46/gl.h>
#include <unordered_map>
#include <vector>

namespace RIS
{
    struct RendererException : public std::runtime_error
	{
		RendererException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    struct ShaderException : public std::runtime_error
    {
        ShaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
    };

    class Shader
    {
    public:
        Shader();
        ~Shader();

        Shader(const Shader &) = delete;
        Shader& operator=(const Shader &) = delete;

        Shader(Shader &&other);
        Shader& operator=(Shader &&other);

        void Create(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type);

        gl::GLuint GetId() const;

    private:
        gl::GLuint programId;

    };

    class ProgramPipeline
    {
    public:
        ProgramPipeline();
        ~ProgramPipeline();

        ProgramPipeline(const ProgramPipeline &) = delete;
        ProgramPipeline& operator=(const ProgramPipeline &) = delete;

        ProgramPipeline(ProgramPipeline &&) = delete;
        ProgramPipeline& operator=(ProgramPipeline &&) = delete;

        void Create();
        void SetShader(const Shader &shader, gl::UseProgramStageMask type);
        void Use();

        gl::GLuint GetId() const;

    private:
        gl::GLuint pipelineId;
    };

    class Texture
    {
    public:
        Texture();
        ~Texture();

        Texture(const Texture &) = delete;
        Texture& operator=(const Texture &) = delete;

        Texture(Texture &&other);
        Texture& operator=(Texture &&other);

        void Create(const std::byte *data, const std::size_t &size);
        void Create(gl::GLenum format, int width, int height);
        void Create(const glm::vec4 color);

        void Bind(gl::GLuint textureUnit);

        gl::GLuint GetId() const;

    private:
        gl::GLuint textureId;
    };

    class Framebuffer
    {
    public:
        Framebuffer();
        Framebuffer(gl::GLuint frambufId);
        ~Framebuffer();

        Framebuffer(const Framebuffer &) = delete;
        Framebuffer& operator=(const Framebuffer &) = delete;

        Framebuffer(Framebuffer &&other);
        Framebuffer& operator=(Framebuffer &&other);

        void Create(int width, int height, gl::GLenum textureFormat, bool useDepth);
        void Resize(int width, int height);
        void Bind();
        void Clear(const glm::vec4 &color, float depth);

        gl::GLuint GetId() const;

        const Texture& GetColorTexture() const;
        const Texture& GetDepthTexture() const;

    private:
        gl::GLuint framebufferId;
        Texture colorTexture;
        Texture depthTexture;

    };

    class Sampler
    {
    public:
        Sampler();
        ~Sampler();

        Sampler(const Sampler &) = delete;
        Sampler& operator=(const Sampler &) = delete;

        Sampler(Sampler &&other);
        Sampler& operator=(Sampler &&other);

        void Create();

        void SetMinFilter(gl::GLenum minFilter);
        void SetMagFilter(gl::GLenum magFilter);
        void SetMaxAnisotropy(float maxAniso);

        void Bind(int textureUnit);

        gl::GLuint GetId() const;

    private:
        gl::GLuint samplerId;

    };

    class Buffer
    {
    public:
        Buffer();
        ~Buffer();

        Buffer(const Buffer &) = delete;
        Buffer& operator=(const Buffer &) = delete;

        Buffer(Buffer &&other);
        Buffer& operator=(Buffer &&other);

        template<typename T>
        void Create(gl::GLenum type, gl::BufferStorageMask usage, const std::vector<T> &data, bool immutable = false);

        template<typename T>
        void UpdateData(const std::vector<T> &data);

        void Bind(int bindBase);

        std::size_t GetElementSize() const;
        gl::GLuint GetId() const;

    private:
        gl::GLuint bufferId;
        bool isImmutable;
        std::size_t elementSize;

    };

    template<typename T>
    void Buffer::Create(gl::GLenum type, gl::BufferStorageMask usage, const std::vector<T> &data, bool immutable)
    {
        using namespace gl46core;
        glCreateBuffers(1, &bufferId);
        isImmutable = immutable;
        elementSize = sizeof(T);
        std::size_t size = data.size * elementSize;
        if(isImmutable)
        {
            glNamedBufferStorage(bufferId, size, data.data(), usage);
        }
        else
        {
            glNamedBufferData(bufferId, size, data.data(), usage);
        }
    }

    template<typename T>
    void Buffer::UpdateData(const std::vector<T> &data)
    {
        if(isImmutable)
            return; // maybe throw an exception here

        elementSize = sizeof(T);
        std::size_t = data.size * elementSize;
        glNamedBufferSubData(bufferId, 0, size, data.data());
    }

    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray &) = delete;
        VertexArray& operator=(const VertexArray &) = delete;

        VertexArray(VertexArray &&other);
        VertexArray& operator=(VertexArray &&other);

        void Create();
        void SetVertexBuffer(const Buffer &buffer, int bindingPoint);
        void SetIndexBuffer(const Buffer &buffer);
        void SetAttribFormat(int attrib, int numComponents, gl::GLenum type, std::size_t offset, int bindingPoint = 0);

        void Bind();

        gl::GLuint GetId() const;

    private:
        gl::GLuint vertexArrayId;

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

        void SetViewsize(int width, int height) override;
        void SetPosition(const glm::vec2 &positino) override;

        void SetTexture(int textureId, int textureUnit) override;
        void SetColor(const glm::vec4 &color) override;

        void Begin() override;
        void End() override; // ??? really needed?

        void DrawText(const std::string &text) override;
        void DrawQuad(int width, int height) override;

    private:
        GLRenderer &renderer;

        glm::mat4 projection;

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

        void Clear(int framebufferId, const glm::vec4 &clearColor) override;

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

        Sampler defaultSampler, uiSampler;

        ProgramPipeline pipeline;
        Shader uiVertex, uiFragment, uiText;

        GL2DRenderer renderer2d;
        Camera camera;

    };
}
