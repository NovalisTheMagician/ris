#include "graphics/Renderer.hpp"
#include "RIS.hpp"

#define GLAD_GL_IMPLEMENTATION
#include <glad2/gl.h>

#include <glm/glm.hpp>

#include <string>
#include <iostream>

#include "misc/Logger.hpp"

using namespace std::literals;

// Windows "hack" to force some laptops to use the highperformance GPU
// some intel integrated gpus don't support opengl 4.5
// I might, in the future, reduce the required opengl version
#ifdef _WIN32
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace RIS::Graphics
{
    Renderer::Renderer()
    {
        auto &log = Logger::Instance();

        if(!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)))
        {
            throw RendererException("Couldn't load GL functions");
        }

        if(!GLAD_GL_VERSION_4_5)
        {
            log.Error("OpenGL version 4.5 not supported");
            throw RendererException("OpenGL version 4.5 or higher not supported");
        }

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        auto dbgCallback = [](GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const void *userParam)
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

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        Config &config = GetConfig();

        int msaa = config.GetValue("r_msaa", 0);
        if(msaa > 0)
            glEnable(GL_MULTISAMPLE);

        log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);

        int width = config.GetValue("r_width", 800);
        int height = config.GetValue("r_height", 600);
        Resize(width, height);
    }

    void Renderer::PostInit()
    {
    }

    void Renderer::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}
