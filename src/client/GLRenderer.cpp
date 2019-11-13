#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    GLRenderer::GLRenderer(const SystemLocator &systems)
        : systems(systems)
    {
        glbinding::initialize(glfwGetProcAddress);

        glEnable(GL_FRAMEBUFFER_SRGB);
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
}
