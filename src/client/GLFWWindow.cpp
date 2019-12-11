#include "GLFWWindow.hpp"

#include "common/Logger.hpp"

#include <exception>

#include <glm/glm.hpp>
#include <gli/gli.hpp>

using std::string;
using namespace std::literals::string_literals;

namespace RIS
{
    GLFWWindow::GLFWWindow(const SystemLocator &systems, Config &config, const string &title)
        : systems(systems), config(config)
    {
        if (!glfwInit())
            throw WindowException("GLFW could't initialize");

        glfwWindowHint(GLFW_SRGB_CAPABLE, true);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); 
#endif

        glfwWindowHint(GLFW_RESIZABLE, false);

        int width = config.GetInt("r_width", 800);
        int height = config.GetInt("r_height", 600);
        int fullscreen = config.GetInt("r_fullscreen", 0);
        bool vsync = config.GetInt("r_vsync", 0);

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
        glfwSetFramebufferSizeCallback(window, GLFWWindow::FramebufferResize);

        glfwMakeContextCurrent(window);
        glfwSwapInterval(vsync);

        //glfwSwapInterval(1);
    }

    GLFWWindow::~GLFWWindow()
    {
        glfwTerminate();
    }

    void  GLFWWindow::SetRelativeMouse(bool setRelative)
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

    void GLFWWindow::SetWindowIcon(const std::string &icon)
    {
        auto &loader = systems.GetLoader();
        try
        {
            size_t size;
            auto asset = loader.LoadAsset(AssetType::TEXTURE, icon, size);
            gli::texture tex = gli::load(reinterpret_cast<const char*>(asset.get()), size);
            if(tex.empty() || tex.format() != gli::format::FORMAT_RGBA8_UNORM_PACK8)
            {
                Logger::Instance().Warning("Window icon (" + icon + ") has an unsupported format"s);
                return;
            }

            glm::tvec3<int> extent(tex.extent(0));
            GLFWimage iconImage = { 0 };
            iconImage.width = extent.x;
            iconImage.height = extent.y;
            iconImage.pixels = reinterpret_cast<unsigned char*>(tex.data(0, 0, 0));

            glfwSetWindowIcon(window, 1, &iconImage);
        }
        catch(const std::exception& e)
        {
            Logger::Instance().Warning("Window icon couldn't be set: "s + e.what());
        }
    }

    void GLFWWindow::SetCursorIcon(const std::string &cursor, int xHot, int yHot)
    {
        auto &loader = systems.GetLoader();
        try
        {
            size_t size;
            auto asset = loader.LoadAsset(AssetType::TEXTURE, cursor, size);
            gli::texture tex = gli::load(reinterpret_cast<const char*>(asset.get()), size);
            if(tex.empty() || tex.format() != gli::format::FORMAT_RGBA8_UNORM_PACK8)
            {
                Logger::Instance().Warning("Window cursor (" + cursor + ") has an unsupported format"s);
                return;
            }

            glm::tvec3<int> extent(tex.extent(0));
            GLFWimage cursorImage = { 0 };
            cursorImage.width = extent.x;
            cursorImage.height = extent.y;
            cursorImage.pixels = reinterpret_cast<unsigned char*>(tex.data(0, 0, 0));

            // maybe cache the cursor object to manually destroy it later
            // but it might not be needed because glfw will destroy any 
            // remaining cursors upon termination
            //
            // this might need some rethinking
            GLFWcursor *cursor = glfwCreateCursor(&cursorImage, xHot, yHot);
            glfwSetCursor(window, cursor);
        }
        catch(const std::exception& e)
        {
            Logger::Instance().Warning("Window cursor couldn't be set: "s + e.what());
        }
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

    GLFWwindow *GLFWWindow::GetWindow() const
    {
        return window;
    }
}
