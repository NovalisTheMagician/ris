#pragma once

#include "GLFWInput.hpp"

namespace RIS
{
    GLFWInput::GLFWInput(const SystemLocator &systems, Config &config, const GLFWWindow &window)
        : systems(systems), config(config)
    {
        GLFWwindow *wnd = window.GetWindow();
    }
}
