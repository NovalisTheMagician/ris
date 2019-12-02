#pragma once

#include "common/IRenderer.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <stdexcept>

#include <glbinding/gl46/gl.h>
#include <unordered_map>

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

        ProgramPipeline(ProgramPipeline &&) = default;
        ProgramPipeline& operator=(ProgramPipeline &&) = default;

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
        void Bind(gl::GLuint textureUnit);

        gl::GLuint GetId() const;

    private:
        gl::GLuint textureId;
    };

    class Framebuffer
    {

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

        void Clear(const glm::vec4 &clearColor) override;
        void Resize(int width, int height) override;

    private:
        const SystemLocator &systems;
        Config &config;

        int highestUnusedTexId;
        std::unordered_map<int, Texture> textures;

        ProgramPipeline pipeline;
        Shader uiVertex, uiFragment, uiText;

    };
}
