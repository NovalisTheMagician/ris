#pragma once

#include "GLFWInput.hpp"

using std::string;

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
        : systems(systems), config(config), keyMap(), ready(false)
    {
        GLFWwindow *wnd = window.GetWindow();

        glfwSetKeyCallback(wnd, KeyCallback);
        glfwSetCharCallback(wnd, CharCallback);
        glfwSetCursorPosCallback(wnd, MousePosCallback);
        glfwSetScrollCallback(wnd, MouseScrollCallback);
        glfwSetMouseButtonCallback(wnd, MouseButtonCallback);
        ready = true;
        instance = this;
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
        if(!ready) return;
        for(auto& [_, callback] : keyUpCallbacks)
            callback(key);
    }

    void GLFWInput::OnKeyDown(int key)
    {
        if(!ready) return;
        for(auto& [_, callback] : keyDownCallbacks)
            callback(key);
    }

    void GLFWInput::OnChar(char character)
    {
        if(!ready) return;
        for(auto& [_, callback] : charCallbacks)
            callback(character);
    }

    void GLFWInput::OnMouseMove(float x, float y)
    {
        if(!ready) return;
        for(auto& [_, callback] : mouseCallbacks)
            callback(x, y);
    }

    void GLFWInput::OnScrollWheel(float x, float y)
    {
        if(!ready) return;
        for(auto& [_, callback] : wheelCallbacks)
            callback(x, y);
    }

    void GLFWInput::OnMouseButtonUp(int button)
    {
        if(!ready) return;
        for(auto& [_, callback] : buttonUpCallbacks)
            callback(button);
    }

    void GLFWInput::OnMouseButtonDown(int button)
    {
        if(!ready) return;
        for(auto& [_, callback] : buttonDownCallbacks)
            callback(button);
    }

    void GLFWInput::RegisterKeyUp(const string &handle, RIS::KeyCallback callback)
    {
        keyUpCallbacks[handle] = callback;
    }

    void GLFWInput::RegisterKeyDown(const string &handle, RIS::KeyCallback callback)
    {
        keyDownCallbacks[handle] = callback;
    }

    void GLFWInput::RegisterButtonUp(const string &handle, RIS::ButtonCallback callback) 
    {
        buttonUpCallbacks[handle] = callback;
    }

    void GLFWInput::RegisterButtonDown(const string &handle, RIS::ButtonCallback callback)
    {
        buttonDownCallbacks[handle] = callback;
    }

    void GLFWInput::RegisterMouse(const string &handle, RIS::MouseCallback callback)
    {
        mouseCallbacks[handle] = callback;
    }

    void GLFWInput::RegisterWheel(const string &handle, RIS::WheelCallback callback)
    {
        wheelCallbacks[handle] = callback;
    }

    void GLFWInput::RegisterChar(const std::string &handle, RIS::CharCallback callback)
    {
        charCallbacks[handle] = callback;
    }

    void GLFWInput::UnregisterKeyUp(const string &handle)
    {
        keyUpCallbacks.erase(handle);
    }

    void GLFWInput::UnregisterKeyDown(const string &handle)
    {
        keyDownCallbacks.erase(handle);
    }

    void GLFWInput::UnregisterButtonUp(const string &handle)
    {
        buttonUpCallbacks.erase(handle);
    }

    void GLFWInput::UnregisterButtonDown(const string &handle)
    {
        buttonDownCallbacks.erase(handle);
    }

    void GLFWInput::UnregisterMouse(const string &handle)
    {
        mouseCallbacks.erase(handle);
    }

    void GLFWInput::UnregisterWheel(const string &handle)
    {
        wheelCallbacks.erase(handle);
    }

    void GLFWInput::UnregisterChar(const string &handle)
    {
        charCallbacks.erase(handle);
    }

    void GLFWInput::Update()
    {

    }
}
