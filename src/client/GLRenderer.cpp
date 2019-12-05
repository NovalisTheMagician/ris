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

#include <iostream>

#include <string>

#include "common/Logger.hpp"

#include "common/ILoader.hpp"

using namespace gl46core;
using namespace std::literals;

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

    Shader::Shader(Shader &&other)
    {
        programId = other.programId;
        other.programId = 0;
    }

    Shader& Shader::operator=(Shader &&other)
    {
        programId = other.programId;
        other.programId = 0;
        return *this;
    }

    Shader::~Shader()
    {
        glDeleteProgram(programId);
    }

    void Shader::Create(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type)
    {
        if(programId != 0)
            glDeleteProgram(programId);

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

    void ProgramPipeline::Create()
    {
        glGenProgramPipelines(1, &pipelineId);
    }

    void ProgramPipeline::SetShader(const Shader &shader, UseProgramStageMask type)
    {
        glUseProgramStages(pipelineId, type, shader.GetId());
    }

    void ProgramPipeline::Use()
    {
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

    Texture::Texture(Texture &&other)
    {
        textureId = other.textureId;
        other.textureId = 0;
    }

    Texture& Texture::operator=(Texture &&other)
    {
        textureId = other.textureId;
        other.textureId = 0;
        return *this;
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &textureId);
    }

    void Texture::Create(const std::byte *data, const std::size_t &size)
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

    void Texture::Create(gl::GLenum format, int width, int height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
        glTextureStorage2D(textureId, 1, format, width, height);
        glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void Texture::Create(const glm::vec4 color)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
        glTextureStorage2D(textureId, 1, GL_RGBA8, 1, 1);
        glTextureSubImage2D(textureId, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, glm::value_ptr(color));
    }

    void Texture::Bind(GLuint textureUnit)
    {
        glBindTextureUnit(textureUnit, textureId);
    }

    GLuint Texture::GetId() const
    {
        return textureId;
    }

// Sampler

    Sampler::Sampler()
        : samplerId(0)
    {
    }

    Sampler::~Sampler()
    {
        glDeleteSamplers(1, &samplerId);
    }

    Sampler::Sampler(Sampler &&other)
    {
        samplerId = other.samplerId;
        other.samplerId = 0;
    }

    Sampler& Sampler::operator=(Sampler &&other)
    {
        samplerId = other.samplerId;
        other.samplerId = 0;
        return *this;
    }

    void Sampler::Create()
    {
        glGenSamplers(1, &samplerId);
    }

    void Sampler::SetMinFilter(GLenum minFilter)
    {
        glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, minFilter);
    }

    void Sampler::SetMagFilter(GLenum magFilter)
    {
        glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, magFilter);
    }

    void Sampler::SetMaxAnisotropy(float maxAniso)
    {
        glSamplerParameterfv(samplerId, GL_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    }

    void Sampler::Bind(int textureUnit)
    {
        glBindSampler(textureUnit, samplerId);
    }

    GLuint Sampler::GetId() const
    {
        return samplerId;
    }

// Framebuffer

    Framebuffer::Framebuffer()
        : framebufferId(0)
    {
    }

    Framebuffer::Framebuffer(GLuint frambufId)
        : framebufferId(frambufId)
    {
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &framebufferId);
    }

    Framebuffer::Framebuffer(Framebuffer &&other)
    {
        framebufferId = other.framebufferId;
        other.framebufferId = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer &&other)
    {
        framebufferId = other.framebufferId;
        other.framebufferId = 0;
        return *this;
    }

    void Framebuffer::Create(int width, int height, GLenum colorFormat, bool useDepth)
    {
        glCreateFramebuffers(1, &framebufferId);
        colorTexture.Create(colorFormat, width, height);
        glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, colorTexture.GetId(), 0);
        if(useDepth)
        {
            depthTexture.Create(GL_DEPTH_COMPONENT32, width, height);
            glNamedFramebufferTexture(framebufferId, GL_DEPTH_ATTACHMENT, depthTexture.GetId(), 0);
        }
    }

    void Framebuffer::Clear(const glm::vec4 &color, float depth)
    {
        glClearNamedFramebufferfv(framebufferId, GL_COLOR, 0, glm::value_ptr(color));
        glClearNamedFramebufferfv(framebufferId, GL_DEPTH, 0, &depth);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    }

    GLuint Framebuffer::GetId() const
    {
        return framebufferId;
    }

    const Texture& Framebuffer::GetColorTexture() const
    {
        return colorTexture;
    }

    const Texture& Framebuffer::GetDepthTexture() const
    {
        return depthTexture;
    }

// Renderer

    const int GLRenderer::DEFAULT_FRAMBUFFER_ID = 0;
    const int GLRenderer::DEFAULT_TEXTURE_ID = 1;
    const int GLRenderer::MISSING_TEXTURE_ID = 0;

    GLRenderer::GLRenderer(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), textures(), highestUnusedTexId(2), framebuffers(), highestUnusedFrambufId(1)
    {
        auto &log = Logger::Instance();

        glbinding::initialize(glfwGetProcAddress);
        //glbinding::aux::enableGetErrorCallback();

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        auto dbgCallback = [](GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const void *userParam) -> void
            {
                if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

                std::cout << "---------------" << std::endl;
                std::cout << "Debug message (" << id << "): " <<  message << std::endl;

                switch (source)
                {
                    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
                    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
                    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
                    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
                    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
                    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
                } std::cout << std::endl;

                switch (type)
                {
                    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
                    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
                    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
                    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
                    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
                    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
                    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
                } std::cout << std::endl;
                
                switch (severity)
                {
                    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
                    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
                    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
                    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
                } std::cout << std::endl;
                std::cout << std::endl;
            };

        glDebugMessageCallback(static_cast<GLDEBUGPROC>(dbgCallback), nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

        glEnable(GL_FRAMEBUFFER_SRGB);

        std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);
    }

    GLRenderer::~GLRenderer()
    {
        textures.clear();
    }

    void GLRenderer::LoadRequiredResources()
    {
        framebuffers.emplace(DEFAULT_FRAMBUFFER_ID, 0);

        Texture defaultTexture;
        defaultTexture.Create(glm::vec4(1, 1, 1, 1));
        Texture missingTexture;
        missingTexture.Create(glm::vec4(1, 0, 1, 1));
        textures.insert(std::make_pair(DEFAULT_TEXTURE_ID, std::move(defaultTexture)));
        textures.insert(std::make_pair(MISSING_TEXTURE_ID, std::move(missingTexture)));

        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);

        float aniso = static_cast<float>(config.GetInt("r_anisotropic", 16));
        if(aniso > maxAniso)
        {
            aniso = maxAniso;
            config.SetValue("r_anisotropic", static_cast<int>(aniso));
        }

        defaultSampler.Create();
        defaultSampler.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
        defaultSampler.SetMagFilter(GL_LINEAR);
        defaultSampler.SetMaxAnisotropy(aniso);

        uiSampler.Create();
        uiSampler.SetMinFilter(GL_LINEAR);
        uiSampler.SetMagFilter(GL_LINEAR);

        ILoader &loader = systems.GetLoader();
        size_t size;
        auto shaderBin = loader.LoadAsset(AssetType::SHADER, "ui_vert", size);
        uiVertex.Create(shaderBin.get(), size, GL_VERTEX_SHADER);

        shaderBin = loader.LoadAsset(AssetType::SHADER, "ui_frag", size);
        uiFragment.Create(shaderBin.get(), size, GL_FRAGMENT_SHADER);

        shaderBin = loader.LoadAsset(AssetType::SHADER, "ui_text", size);
        uiText.Create(shaderBin.get(), size, GL_FRAGMENT_SHADER);
    }

    int GLRenderer::LoadTexture(const std::string &name)
    {
        ILoader &loader = systems.GetLoader();
        try
        {
            std::size_t size;
            auto data = loader.LoadAsset(AssetType::TEXTURE, name, size);

            Texture texture;
            texture.Create(data.get(), size);
            int id = highestUnusedTexId++;
            textures[id] = std::move(texture);
            return id;
        }
        catch(const std::exception& e)
        {
            Logger::Instance().Error("Failed to load texture (" + name + "): "s + e.what());
            return MISSING_TEXTURE_ID;
        }
    }

    void GLRenderer::DestroyTexture(int texId)
    {
        if(texId >= 0 && textures.count(texId) > 0)
        {
            textures.erase(texId);
        }
    }

    int GLRenderer::CreateFramebuffer(int width, int height, bool useDepth)
    {
        int id = highestUnusedFrambufId++;

        int framebufferWidth = width;
        int framebufferHeight = height;
        if(width == -1)
            framebufferWidth = config.GetInt("r_width", 800);
        if(height == -1)
            framebufferHeight = config.GetInt("r_height", 600);

        Framebuffer framebuffer;
        framebuffer.Create(framebufferWidth, framebufferHeight, GL_RGBA, useDepth);
        framebuffers[id] = std::move(framebuffer);

        return id;
    }

    void GLRenderer::DestroyFramebuffer(int framebufId)
    {
        if(framebufId > 0 && framebuffers.count(framebufId) > 0)
        {
            framebuffers.erase(framebufId);
        }
    }

    void GLRenderer::SetFramebuffer(int framebufferId)
    {
        if(framebufferId >= 0 && framebuffers.count(framebufferId) > 0)
        {
            Framebuffer &framebuffer = framebuffers.at(framebufferId);
            framebuffer.Bind();
        }
    }

    void GLRenderer::Begin(ProjectionType type)
    {
        if(type == ProjectionType::ORTHOGRAPHIC)
        {
            pipeline.SetShader(uiVertex, GL_VERTEX_SHADER_BIT);
        }
        else if(type == ProjectionType::PERSPECTIVE)
        {

        }
    }

    void GLRenderer::End()
    {

    }

    void GLRenderer::Clear(int framebufferId, const glm::vec4 &clearColor)
    {
        Framebuffer &framebuffer = framebuffers.at(framebufferId);
        framebuffer.Clear(clearColor, 1.0f);
    }

    void GLRenderer::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}
