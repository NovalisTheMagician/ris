#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gli/gli.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "common/Logger.hpp"

#include "common/ILoader.hpp"

#include "VertexTypes.hpp"

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
    const int GLRenderer::DEFAULT_FRAMBUFFER_ID = 0;
    const int GLRenderer::DEFAULT_TEXTURE_ID = 1;
    const int GLRenderer::MISSING_TEXTURE_ID = 0;

    GLRenderer::GLRenderer(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), textures(), highestUnusedTexId(2), framebuffers(), highestUnusedFrambufId(1), renderer2d(*this)
    {
        auto &log = Logger::Instance();

        glbinding::initialize(glfwGetProcAddress);

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
                auto const src_str = [source]() {
                    switch (source)
                    {
                    case GL_DEBUG_SOURCE_API: return "API";
                    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
                    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
                    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
                    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
                    case GL_DEBUG_SOURCE_OTHER: return "OTHER";
                    default: return "";
                    }
                }();

                auto const type_str = [type]() {
                    switch (type)
                    {
                    case GL_DEBUG_TYPE_ERROR: return "ERROR";
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
                    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
                    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
                    case GL_DEBUG_TYPE_MARKER: return "MARKER";
                    case GL_DEBUG_TYPE_OTHER: return "OTHER";
                    default: return "";
                    }
                }();

                auto const severity_str = [severity]() {
                    switch (severity) {
                    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
                    case GL_DEBUG_SEVERITY_LOW: return "LOW";
                    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
                    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
                    default: return "";
                    }
                }();

                std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << std::endl;
            };

        glDebugMessageCallback(static_cast<GLDEBUGPROC>(dbgCallback), nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);

        renderer2d.Setup();

        int width = config.GetInt("r_width", 800);
        int height = config.GetInt("r_height", 600);
        Resize(width, height);

        framebuffers.emplace(DEFAULT_FRAMBUFFER_ID, 0);

        textures.insert(std::make_pair(DEFAULT_TEXTURE_ID, Texture::Create(glm::vec4(1, 1, 1, 1))));
        textures.insert(std::make_pair(MISSING_TEXTURE_ID, Texture::Create(glm::vec4(1, 0, 1, 1))));

        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);

        float aniso = static_cast<float>(config.GetInt("r_anisotropic", static_cast<int>(maxAniso)));
        if(aniso > maxAniso)
        {
            aniso = maxAniso;
            config.SetValue("r_anisotropic", static_cast<int>(aniso));
        }

        defaultSampler = Sampler::Create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, aniso);
        uiSampler = Sampler::Create(GL_LINEAR, GL_LINEAR, 1.0f);

        pipeline = ProgramPipeline::Create();

        std::vector<VertexType::UIVertex> vertices(6);
        vertices[0] = {{1, 1}, {1, 1}};
        vertices[1] = {{-1, 1}, {0, 1}};
        vertices[2] = {{-1, -1}, {0, 0}};
        vertices[3] = {{1, -1}, {1, 0}};
        vertices[4] = {{1, 1}, {1, 1}};
        vertices[5] = {{-1, -1}, {0, 0}};
        fullscreenQuad = Buffer<VertexType::UIVertex>::CreateImmutable(vertices, GL_DYNAMIC_STORAGE_BIT);

        postprocessVAO = VertexArray::Create();
        postprocessVAO.SetAttribFormat(0, 2, GL_FLOAT, offsetof(VertexType::UIVertex, position));
        postprocessVAO.SetAttribFormat(1, 2, GL_FLOAT, offsetof(VertexType::UIVertex, texCoords));
        postprocessVAO.SetVertexBuffer(fullscreenQuad, 0);

        GLenum enc;
        glGetNamedFramebufferAttachmentParameteriv(0, GL_FRONT_LEFT, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &enc);
        if(enc != GL_SRGB)
        {
            Logger::Instance().Warning("Window is not sRGB capable!");
        }
    }

    GLRenderer::~GLRenderer()
    {
        textures.clear();
    }

    void GLRenderer::LoadRequiredResources()
    {
        ILoader &loader = systems.GetLoader();
        size_t size;
        auto shaderBin = loader.LoadAsset(AssetType::SHADER, "uiVert", size);
        uiVertex = Shader::Create(shaderBin.get(), size, GL_VERTEX_SHADER);

        shaderBin = loader.LoadAsset(AssetType::SHADER, "uiFrag", size);
        uiFragment = Shader::Create(shaderBin.get(), size, GL_FRAGMENT_SHADER);

        shaderBin = loader.LoadAsset(AssetType::SHADER, "uiText", size);
        uiText = Shader::Create(shaderBin.get(), size, GL_FRAGMENT_SHADER);

        shaderBin = loader.LoadAsset(AssetType::SHADER, "ppVert", size);
        ppVertex = Shader::Create(shaderBin.get(), size, GL_VERTEX_SHADER);

        shaderBin = loader.LoadAsset(AssetType::SHADER, "ppCopy", size);
        ppCopy = Shader::Create(shaderBin.get(), size, GL_FRAGMENT_SHADER);
    }

    int GLRenderer::LoadTexture(const std::string &name, bool flip)
    {
        ILoader &loader = systems.GetLoader();
        try
        {
            std::size_t size;
            auto data = loader.LoadAsset(AssetType::TEXTURE, name, size);

            int id = highestUnusedTexId++;
            textures[id] = Texture::Create(data.get(), size, flip);
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

        //GL_SRGB8_ALPHA8
        //GL_RGBA8

        framebuffers[id] = Framebuffer::Create(framebufferWidth, framebufferHeight, GL_RGBA8, useDepth);

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

    void GLRenderer::FramebufferResize(int framebufferId, int width, int height)
    {
        if(framebufferId > 0 && framebuffers.count(framebufferId) > 0)
        {
            Framebuffer &framebuffer = framebuffers.at(framebufferId);
            framebuffer.Resize(width, height);
        }
    }

    void GLRenderer::Clear(int framebufferId, const glm::vec4 &clearColor, float depth)
    {
        Framebuffer &framebuffer = framebuffers.at(framebufferId);
        framebuffer.Clear(clearColor, depth);
    }

    void GLRenderer::Draw(int framebufferId)
    {
        if(framebufferId == DEFAULT_FRAMBUFFER_ID || framebuffers.count(framebufferId) != 1)
            return;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        auto &framebuffer = framebuffers.at(framebufferId);
        framebuffer.GetColorTexture().Bind(0);
        uiSampler.Bind(0);

        postprocessVAO.Bind();

        pipeline.SetShader(ppVertex);
        pipeline.SetShader(ppCopy);
        pipeline.Use();

        SetFramebuffer(DEFAULT_FRAMBUFFER_ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void GLRenderer::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
        renderer2d.SetViewsize(width, height);
    }

    I2DRenderer& GLRenderer::Get2DRenderer()
    {
        return renderer2d;
    }
}
