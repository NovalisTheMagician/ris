#include "GLFWWindow.hpp"

#include <exception>

using std::string;
using namespace std::literals::string_literals;

namespace RIS
{
    GLFWWindow::GLFWWindow(const SystemLocator &systems, const string &title)
        : systems(systems)
    {
        if (!glfwInit())
            throw WindowException("GLFW could't initialize");

        glfwWindowHint(GLFW_SRGB_CAPABLE, true);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        window = glfwCreateWindow(800, 600, title.c_str(), nullptr, nullptr);
        if (!window)
        {
            const char *errorString;
            int ec = glfwGetError(&errorString);
            glfwTerminate();
            throw WindowException("Window creation failed (Error code: "s + std::to_string(ec) + "): "s + errorString);
        }

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, GLFWWindow::FramebufferResize);

        glfwMakeContextCurrent(window);
    }

    GLFWWindow::~GLFWWindow()
    {
        glfwTerminate();
    }

    bool GLFWWindow::HandleMessages()
    {
        glfwPollEvents();
        return glfwWindowShouldClose(window);
    }

    void GLFWWindow::Present()
    {
        glfwSwapBuffers(window);
    }

    void GLFWWindow::FramebufferResize(GLFWwindow *window, int width, int height)
    {
        GLFWWindow *wnd = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        IRenderer &renderer = wnd->systems.GetRenderer();
        renderer.Resize(width, height);
    }
}
