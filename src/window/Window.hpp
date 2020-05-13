#pragma once

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>
#include <memory>

#include "graphics/Image.hpp"

namespace RIS
{
    namespace Window
    {
        struct WindowException : public std::runtime_error
        {
            WindowException(std::string reason) : std::runtime_error(reason.c_str()) {}
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

            void Exit(int exitCode);

            void RegisterScriptFunctions();

            bool HandleMessages();
            void Present();

            GLFWwindow *GetWindowHandle() const;

        private:
            static void FramebufferResize(GLFWwindow *window, int width, int height);

        private:
            GLFWwindow *window;

        };
    }
}