#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>
#include <glbinding-aux/debug.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gli/gli.hpp>

#include <string>

#include "common/Logger.hpp"

#include "common/ILoader.hpp"

using namespace gl46core;

// Windows "hack" to force some laptops to use the highperformance GPU
// some intel integrated gpus don't support opengl 4.6
// I might, in the future, reduce the required opengl version
#ifdef _WIN32
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace RIS
{
// Shader

    Shader::Shader()
        : programId(0)
    {

    }

    Shader::Shader(const std::byte *shaderBinary, const std::size_t &size, GLenum type)
        : programId(0)
    {
        programId = glCreateProgram();
        GLuint shader = glCreateShader(type);
        glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary, size);
        glSpecializeShader(shader, "main", 0, nullptr, nullptr);

        GLenum status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(!static_cast<bool>(status))
        {
            GLenum loglen;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(static_cast<int>(loglen), '#');
                glGetShaderInfoLog(shader, static_cast<GLsizei>(loglen), 0, &errorLog[0]);
                throw ShaderException("Failed to load shader: " + errorLog);
            }

            throw ShaderException("Failed to load shader");
        }

        glProgramParameteri(programId, GL_PROGRAM_SEPARABLE, GL_TRUE);
        glAttachShader(programId, shader);
        glLinkProgram(programId);

        glGetProgramiv(programId, GL_LINK_STATUS, &status);
        if(!static_cast<bool>(status))
        {
            GLenum loglen;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(static_cast<int>(loglen), '#');
                glGetProgramInfoLog(shader, static_cast<GLsizei>(loglen), 0, &errorLog[0]);
                throw ShaderException("Failed to link shader: " + errorLog);
            }

            throw ShaderException("Failed to link shader");
        }

        glDetachShader(programId, shader);
        glDeleteShader(shader);
    }

    Shader::~Shader()
    {
        glDeleteProgram(programId);
    }

    GLuint Shader::GetId() const
    {
        return programId;
    }

// ProgramPipeline

    ProgramPipeline::ProgramPipeline()
        : pipelineId(0)
    {
        
    }

    ProgramPipeline::~ProgramPipeline()
    {
        glDeleteProgramPipelines(1, &pipelineId);
    }

    void ProgramPipeline::SetShader(const Shader &shader, UseProgramStageMask type)
    {
        if(pipelineId == 0)
            glGenProgramPipelines(1, &pipelineId);
        glUseProgramStages(pipelineId, type, shader.GetId());
    }

    void ProgramPipeline::Use()
    {
        if(pipelineId == 0)
            glGenProgramPipelines(1, &pipelineId);
        glBindProgramPipeline(pipelineId);
    }

    GLuint ProgramPipeline::GetId() const
    {
        return pipelineId;
    }

// Texture

    Texture::Texture()
        : textureId(0)
    {

    }

    Texture::Texture(const std::byte *data, const std::size_t &size)
        : textureId(0)
    {
        gli::texture texture = gli::load(reinterpret_cast<const char*>(data), size);
        if(texture.empty())
            return;

        gli::gl gl(gli::gl::PROFILE_GL33);
        gli::gl::format const format = gl.translate(texture.format(), texture.swizzles());
        glm::tvec3<GLsizei> extent(texture.extent(0));
        GLenum target = static_cast<GLenum>(gl.translate(texture.target()));

        glCreateTextures(target, 1, &textureId);
        glTextureParameteri(textureId, GL_TEXTURE_BASE_LEVEL, 0);
        glTextureParameteri(textureId, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
        glTextureParameteriv(textureId, GL_TEXTURE_SWIZZLE_RGBA, &format.Swizzles[0]);
        glTextureStorage2D(textureId, static_cast<GLint>(texture.levels()), static_cast<GLenum>(format.Internal), extent.x, extent.y);
        for(std::size_t level = 0; level < texture.levels(); ++level)
        {
            extent = glm::tvec3<GLsizei>(texture.extent(level));
            glCompressedTextureSubImage2D(textureId, static_cast<GLint>(level), 0, 0, extent.x, extent.y, static_cast<GLenum>(format.Internal), static_cast<GLsizei>(texture.size(level)), texture.data(0, 0, level));
        }
        
        if(texture.levels() == 1)
        {
            glGenerateTextureMipmap(textureId);
        }
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &textureId);
    }

    void Texture::Bind(GLuint textureUnit)
    {
        glBindTextureUnit(textureUnit, textureId);
    }

    GLuint Texture::GetId() const
    {
        return textureId;
    }

// Renderer

    GLRenderer::GLRenderer(const SystemLocator &systems, Config &config)
        : systems(systems), config(config)
    {
        glbinding::initialize(glfwGetProcAddress);
        glbinding::aux::enableGetErrorCallback();

        glEnable(GL_FRAMEBUFFER_SRGB);

        std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        auto &log = Logger::Instance();
        log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);
    }

    GLRenderer::~GLRenderer()
    {

    }

    void GLRenderer::LoadRequiredResources()
    {
        ILoader &loader = systems.GetLoader();
        size_t size;
        auto shaderBin = loader.LoadAsset(AssetType::SHADER, "ui_vert", size);
        uiVertex = std::move(Shader(shaderBin.get(), size, GL_VERTEX_SHADER));

        shaderBin = loader.LoadAsset(AssetType::SHADER, "ui_frag", size);
        uiFragment = std::move(Shader(shaderBin.get(), size, GL_FRAGMENT_SHADER));

        shaderBin = loader.LoadAsset(AssetType::SHADER, "ui_text", size);
        uiText = std::move(Shader(shaderBin.get(), size, GL_FRAGMENT_SHADER));
    }

    void GLRenderer::Clear(const glm::vec4 &clearColor)
    {
        static const float clearDepth = 1.0f;

        glClearNamedFramebufferfv(0, GL_COLOR, 0, glm::value_ptr(clearColor));
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clearDepth);
    }

    void GLRenderer::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}
