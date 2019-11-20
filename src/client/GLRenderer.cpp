#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "common/Logger.hpp"

using namespace gl46core;

namespace RIS
{
    GLRenderer::GLRenderer(const SystemLocator &systems)
        : systems(systems)
    {
        glbinding::initialize(glfwGetProcAddress);

        glEnable(GL_FRAMEBUFFER_SRGB);

        std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        auto &log = Logger::Instance();
        log.Info("GL_Version: " + version);
        log.Info("GL_Vendor: " + vendor);
        log.Info("GL_ShaderVersion: " + shaderVersion);
        log.Info("GL_Renderer: " + renderer);
    }

    GLRenderer::~GLRenderer()
    {

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
