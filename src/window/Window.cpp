#include "window/Window.hpp"

#include "RIS.hpp"

#include "misc/Logger.hpp"
#include "script/ScriptEngine.hpp"
#include "graphics/Renderer.hpp"

#include <exception>

#include <glm/glm.hpp>
#include <gli/gli.hpp>

using std::string;
using namespace std::literals::string_literals;

#include <iostream>

namespace RIS
{
    namespace Window
    {
        Window::Window(const string &title)
        {
            if (!glfwInit())
                throw WindowException("GLFW could't initialize");

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

            glfwWindowHint(GLFW_RESIZABLE, false);

            Config &config = GetConfig();

            int width = config.GetValue("r_width", 800);
            int height = config.GetValue("r_height", 600);
            int fullscreen = config.GetValue("r_fullscreen", 0);
            bool vsync = config.GetValue("r_vsync", false);

            glfwWindowHint(GLFW_RED_BITS, 8);
            glfwWindowHint(GLFW_GREEN_BITS, 8);
            glfwWindowHint(GLFW_BLUE_BITS, 8);
            glfwWindowHint(GLFW_ALPHA_BITS, 8);

            if(fullscreen == 0) // windowed mode
            {
                window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
            }
            else if(fullscreen == 1) // exclusive fullscreen mode
            {
                GLFWmonitor *monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode *mode = glfwGetVideoMode(monitor);

                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

                window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
            }
            else // borderless fullscreen window mode
            {
                GLFWmonitor *monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode *mode = glfwGetVideoMode(monitor);

                glfwWindowHint(GLFW_DECORATED, false);

                window = glfwCreateWindow(mode->width, mode->height, title.c_str(), nullptr, nullptr);

                config.SetValue("r_width", mode->width);
                config.SetValue("r_height", mode->height);
            }
            
            if (!window)
            {
                const char *errorString;
                int ec = glfwGetError(&errorString);
                glfwTerminate();
                throw WindowException("Window creation failed (Error code: "s + std::to_string(ec) + "): "s + errorString);
            }

            glfwSetWindowUserPointer(window, this);
            glfwSetFramebufferSizeCallback(window, Window::FramebufferResize);

            glfwMakeContextCurrent(window);
            glfwSwapInterval(vsync);
        }

        Window::~Window()
        {
            glfwTerminate();
        }

        void Window::PostInit()
        {
            
        }

        void Window::RegisterScriptFunctions()
        {
            auto &scriptEngine = GetScriptEngine();

            scriptEngine.Register<void(), 0>("G_Exit", [this]()
            {
                Exit(0);
            });
            scriptEngine.Register<void(int, const char*), 1>("G_Log", [](int severity, const char *msg)
            {
                if(severity == 0)
                    Logger::Instance().Info(msg);
                else if(severity == 1)
                    Logger::Instance().Warning(msg);
                else
                    Logger::Instance().Error(msg);
            });

            scriptEngine.Register<void(bool)>("G_SetRelativeMouse", [this](bool relative){ SetRelativeMouse(relative); });
        }

        void  Window::SetRelativeMouse(bool setRelative)
        {
            if(setRelative)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                if(glfwRawMouseMotionSupported())
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, true);
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }

        void Window::SetWindowIcon(std::shared_ptr<Graphics::Image> icon)
        {
            GLFWimage iconImage = { 0 };
            iconImage.width = icon->GetWidth();
            iconImage.height = icon->GetHeight();
            iconImage.pixels = icon->GetPixels();

            glfwSetWindowIcon(window, 1, &iconImage);
        }

        void Window::SetCursorIcon(std::shared_ptr<Graphics::Image> cursor, int xHot, int yHot)
        {
            GLFWimage cursorImage = { 0 };
            cursorImage.width = cursor->GetWidth();
            cursorImage.height = cursor->GetHeight();
            cursorImage.pixels = cursor->GetPixels();

            // maybe cache the cursor object to manually destroy it later
            // but it is not needed because glfw will destroy any 
            // remaining cursors upon termination
            //
            // this might need some rethinking
            GLFWcursor *glfwcursor = glfwCreateCursor(&cursorImage, xHot, yHot);
            glfwSetCursor(window, glfwcursor);
        }

        void Window::Exit(int exitCode)
        {
            glfwSetWindowShouldClose(window, true);
        }

        bool Window::HandleMessages()
        {
            glfwPollEvents();
            return glfwWindowShouldClose(window);
        }

        void Window::Present()
        {
            glfwSwapBuffers(window);
        }

        void Window::FramebufferResize(GLFWwindow *window, int width, int height)
        {
            Window *wnd = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            auto &renderer = GetRenderer();
            renderer.Resize(width, height);
        }

        GLFWwindow *Window::GetWindowHandle() const
        {
            return window;
        }
    }
}
