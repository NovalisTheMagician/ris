#include "GLFWWindow.hpp"

#include <exception>

using std::string;

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
            glfwTerminate();
            throw WindowException("Couldn't create window");
        }

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
}
