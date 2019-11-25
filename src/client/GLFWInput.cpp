#pragma once

#include "GLFWInput.hpp"

namespace RIS
{
    const std::unordered_map<int, InputAction> GLFWInput::DEFAULT_KEYMAP = 
    {
        { GLFW_KEY_W, InputAction::MOVE_FORWARD },
        { GLFW_KEY_S, InputAction::MOVE_BACKWARD },
        { GLFW_KEY_A, InputAction::MOVE_LEFT },
        { GLFW_KEY_D, InputAction::MOVE_RIGHT },
        { GLFW_KEY_SPACE, InputAction::JUMP },
        { GLFW_KEY_LEFT_CONTROL, InputAction::CROUCH },
        { GLFW_KEY_LEFT_ALT, InputAction::RUN },
        { GLFW_KEY_E, InputAction::USE },
    };

    GLFWInput *GLFWInput::instance = nullptr;

    GLFWInput::GLFWInput(const SystemLocator &systems, Config &config, const GLFWWindow &window)
        : systems(systems), config(config), keyMap()
    {
        GLFWwindow *wnd = window.GetWindow();

        glfwSetKeyCallback(wnd, KeyCallback);
        glfwSetCharCallback(wnd, CharCallback);
        glfwSetCursorPosCallback(wnd, MousePosCallback);
        glfwSetScrollCallback(wnd, MouseScrollCallback);
        glfwSetMouseButtonCallback(wnd, MouseButtonCallback);
    }

    GLFWInput::~GLFWInput()
    {

    }

    void GLFWInput::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        GLFWInput &input = *instance;
        if(action == GLFW_PRESS)
        {
            input.OnKeyDown(key);
        }
        else if(action == GLFW_RELEASE)
        {
            input.OnKeyUp(key);
        }
    }

    void GLFWInput::CharCallback(GLFWwindow *window, unsigned int codePoint)
    {
        GLFWInput &input = *instance;
        input.OnChar(static_cast<char>(codePoint));
    }

    void GLFWInput::MousePosCallback(GLFWwindow* window, double x, double y)
    {
        GLFWInput &input = *instance;
        input.OnMouseMove(static_cast<float>(x), static_cast<float>(y));
    }

    void GLFWInput::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        GLFWInput &input = *instance;
        if(action == GLFW_PRESS)
        {
            input.OnMouseButtonDown(button);
        }
        else if(action == GLFW_RELEASE)
        {
            input.OnMouseButtonUp(button);
        }
    }
    
    void GLFWInput::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        GLFWInput &input = *instance;
        input.OnScrollWheel(static_cast<float>(xoffset), static_cast<float>(yoffset));
    }

    void GLFWInput::OnKeyUp(int key)
    {

    }

    void GLFWInput::OnKeyDown(int key)
    {

    }

    void GLFWInput::OnChar(char character)
    {

    }

    void GLFWInput::OnMouseMove(float x, float y)
    {

    }

    void GLFWInput::OnScrollWheel(float x, float y)
    {

    }

    void GLFWInput::OnMouseButtonUp(int button)
    {

    }

    void GLFWInput::OnMouseButtonDown(int button)
    {

    }

    void GLFWInput::Update()
    {

    }
}
