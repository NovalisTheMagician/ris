#include "window/Window.hpp"

#include "RIS.hpp"

#include "misc/Logger.hpp"
#include "misc/Version.hpp"
#include "script/ScriptEngine.hpp"
#include "graphics/Renderer.hpp"
#include "ui/Userinterface.hpp"

#include <exception>

#include <glm/glm.hpp>
#include <gli/gli.hpp>

#include <fmt/format.h>

using std::string;
using namespace std::literals::string_literals;

#include <iostream>

namespace RIS::Window
{
    Window::Window(const string &title)
    {
        if (!glfwInit())
            throw WindowException("GLFW could't initialize");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
        int msaa = config.GetValue("r_msaa", 0);

        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        glfwWindowHint(GLFW_SAMPLES, msaa);

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
            throw WindowException(fmt::format("Window creation failed (Error code: {}): {}", std::to_string(ec), errorString));
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
        auto &console = GetUserinterface().GetConsole();
        console.BindFunc("exit", [this](std::vector<std::string> params){ Exit(0); return ""; });
        console.BindFunc("vsync", [this](std::vector<std::string> params)
        { 
            if(params.size() == 0)
            {
                const auto &config = GetConfig();
                return std::to_string(config.GetValue("r_vsync", false));
            }
            else
            {
                try
                {
                    bool value = std::stoi(params.at(0));
                    SetVsync(value);
                }
                catch(const std::exception& e)
                {
                    return "Invalid Value"s;
                }
            }
            return std::string(); 
        });
    }

    void Window::RegisterScriptFunctions()
    {
        /*
        auto &scriptEngine = GetScriptEngine();

        scriptEngine.Namespace("Window")
            .Func("exit", [this](){ Exit(0); })
            .Func("logInfo", [](const std::string &msg){ Logger::Instance().Info(msg); })
            .Func("logWarning", [](const std::string &msg){ Logger::Instance().Warning(msg); })
            .Func("logError", [](const std::string &msg){ Logger::Instance().Error(msg); })
            .Func("setRelativeMouse", [this](bool relative){ SetRelativeMouse(relative); });
        
        scriptEngine.Namespace("Game")
            .Func("getGameName", [](){ return Version::GAME_NAME; })
            .Func("getVersionMajor", [](){ return Version::MAJOR; })
            .Func("getVersionMinor", [](){ return Version::MINOR; });
        */
    }

    void Window::SetRelativeMouse(bool setRelative)
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

    std::string Window::GetClipboard() const
    {
        auto clipString = glfwGetClipboardString(nullptr);
        if(clipString)
            return std::string(clipString);
        return std::string();
    }

    void Window::SetClipboard(const std::string &str) const
    {
        glfwSetClipboardString(nullptr, str.c_str());
    }

    void Window::RequestAttention()
    {
        glfwRequestWindowAttention(window);
    }

    void Window::SetVsync(bool vsync)
    {
        auto &config = GetConfig();
        config.SetValue("r_vsync", vsync);
        glfwSwapInterval(vsync);
    }
}
