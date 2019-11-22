#pragma once

#include "common/IWindow.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#undef APIENTRY

#include <stdexcept>
#include <string>

namespace RIS
{
    struct WindowException : public std::runtime_error
	{
		WindowException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class GLFWWindow : public IWindow
    {
    public:
        GLFWWindow(const SystemLocator &systems, Config &config, const std::string &title);
        ~GLFWWindow();

        GLFWWindow(const GLFWWindow &) = delete;
        GLFWWindow(GLFWWindow &&) = delete;
        GLFWWindow &operator=(const GLFWWindow &) = delete;
        GLFWWindow &operator=(GLFWWindow &&) = delete;

        bool HandleMessages() override;
        void Present() override;

        GLFWwindow *GetWindow() const;

    private:
        static void FramebufferResize(GLFWwindow *window, int width, int height);

    private:
        const SystemLocator &systems;
        Config &config;
        GLFWwindow *window;

    };
}