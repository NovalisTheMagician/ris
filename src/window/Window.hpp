#pragma once

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "RisExcept.hpp"

#include <string>
#include <memory>

#include "graphics/Image.hpp"

namespace RIS::Window
{
    struct WindowException : public RISException
    {
        WindowException(const std::string &reason) : RISException(reason) {}
    };

    class Window
    {
    public:
        Window(const std::string &title);
        ~Window();

        Window(const Window &) = delete;
        Window(Window &&) = delete;
        Window &operator=(const Window &) = delete;
        Window &operator=(Window &&) = delete;

        void PostInit();

        void SetRelativeMouse(bool setRelative);

        void SetWindowIcon(std::shared_ptr<Graphics::Image> icon);
        void SetCursorIcon(std::shared_ptr<Graphics::Image> cursor, int xHot, int yHot);

        std::string GetClipboard() const;
        void SetClipboard(const std::string &str) const;

        void RequestAttention();

        void SetVsync(bool vsync);

        void Exit(int exitCode);

        void RegisterScriptFunctions();

        bool HandleMessages();
        void Present();

        GLFWwindow *GetWindowHandle() const;

    private:
        static void FramebufferResize(GLFWwindow *window, int width, int height);

        void CenterWindow(GLFWwindow *window, GLFWmonitor *monitor);

    private:
        GLFWwindow *window;

    };
}