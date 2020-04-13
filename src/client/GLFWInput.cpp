#pragma once

#include "GLFWInput.hpp"

using std::string;

namespace RIS
{
    GLFWInput *GLFWInput::instance = nullptr;

    GLFWInput::GLFWInput(const SystemLocator &systems, Config &config, const GLFWWindow &window)
        : systems(systems), config(config), ready(false)
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
            callback(TranslateKey(key));
    }

    void GLFWInput::OnKeyDown(int key)
    {
        if(!ready) return;
        for(auto& [_, callback] : keyDownCallbacks)
            callback(TranslateKey(key));
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
            callback(TranslateButton(button));
    }

    void GLFWInput::OnMouseButtonDown(int button)
    {
        if(!ready) return;
        for(auto& [_, callback] : buttonDownCallbacks)
            callback(TranslateButton(button));
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

    InputKey GLFWInput::TranslateKey(int key)
    {
        switch(key)
        {
        case GLFW_KEY_SPACE: return InputKey::SPACE;
        case GLFW_KEY_APOSTROPHE: return InputKey::APOSTROPHE;
        case GLFW_KEY_COMMA: return InputKey::COMMA;
        case GLFW_KEY_MINUS: return InputKey::MINUS;
        case GLFW_KEY_PERIOD: return InputKey::PERIOD;
        case GLFW_KEY_SLASH: return InputKey::SLASH;
        case GLFW_KEY_0: return InputKey::NUM_0;
        case GLFW_KEY_1: return InputKey::NUM_1;
        case GLFW_KEY_2: return InputKey::NUM_2;
        case GLFW_KEY_3: return InputKey::NUM_3;
        case GLFW_KEY_4: return InputKey::NUM_4;
        case GLFW_KEY_5: return InputKey::NUM_5;
        case GLFW_KEY_6: return InputKey::NUM_6;
        case GLFW_KEY_7: return InputKey::NUM_7;
        case GLFW_KEY_8: return InputKey::NUM_8;
        case GLFW_KEY_9: return InputKey::NUM_9;
        case GLFW_KEY_SEMICOLON: return InputKey::SEMICOLON;
        case GLFW_KEY_EQUAL: return InputKey::EQUALS;
        case GLFW_KEY_A: return InputKey::A;
        case GLFW_KEY_B: return InputKey::B;
        case GLFW_KEY_C: return InputKey::C;
        case GLFW_KEY_D: return InputKey::D;
        case GLFW_KEY_E: return InputKey::E;
        case GLFW_KEY_F: return InputKey::F;
        case GLFW_KEY_G: return InputKey::G;
        case GLFW_KEY_H: return InputKey::H;
        case GLFW_KEY_I: return InputKey::I;
        case GLFW_KEY_J: return InputKey::J;
        case GLFW_KEY_K: return InputKey::K;
        case GLFW_KEY_L: return InputKey::L;
        case GLFW_KEY_M: return InputKey::M;
        case GLFW_KEY_N: return InputKey::N;
        case GLFW_KEY_O: return InputKey::O;
        case GLFW_KEY_P: return InputKey::P;
        case GLFW_KEY_Q: return InputKey::Q;
        case GLFW_KEY_R: return InputKey::R;
        case GLFW_KEY_S: return InputKey::S;
        case GLFW_KEY_T: return InputKey::T;
        case GLFW_KEY_U: return InputKey::U;
        case GLFW_KEY_V: return InputKey::V;
        case GLFW_KEY_W: return InputKey::W;
        case GLFW_KEY_X: return InputKey::X;
        case GLFW_KEY_Y: return InputKey::Y;
        case GLFW_KEY_Z: return InputKey::Z;
        case GLFW_KEY_LEFT_BRACKET: return InputKey::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return InputKey::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return InputKey::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return InputKey::GRAVE_ACCENT;
        case GLFW_KEY_WORLD_1: return InputKey::WORLD_1;
        case GLFW_KEY_WORLD_2: return InputKey::WORLD_2;
        case GLFW_KEY_ESCAPE: return InputKey::ESCAPE;
        case GLFW_KEY_ENTER: return InputKey::ENTER;
        case GLFW_KEY_TAB: return InputKey::TAB;
        case GLFW_KEY_BACKSPACE: return InputKey::BACKSPACE;
        case GLFW_KEY_INSERT: return InputKey::INSERT;
        case GLFW_KEY_DELETE: return InputKey::DELETE;
        case GLFW_KEY_RIGHT: return InputKey::RIGHT;
        case GLFW_KEY_LEFT: return InputKey::LEFT;
        case GLFW_KEY_DOWN: return InputKey::DOWN;
        case GLFW_KEY_UP: return InputKey::UP;
        case GLFW_KEY_PAGE_UP: return InputKey::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return InputKey::PAGE_DOWN;
        case GLFW_KEY_HOME: return InputKey::HOME;
        case GLFW_KEY_END: return InputKey::END;
        case GLFW_KEY_CAPS_LOCK: return InputKey::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return InputKey::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return InputKey::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return InputKey::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return InputKey::PAUSE;
        case GLFW_KEY_F1: return InputKey::F1;
        case GLFW_KEY_F2: return InputKey::F2;
        case GLFW_KEY_F3: return InputKey::F3;
        case GLFW_KEY_F4: return InputKey::F4;
        case GLFW_KEY_F5: return InputKey::F5;
        case GLFW_KEY_F6: return InputKey::F6;
        case GLFW_KEY_F7: return InputKey::F7;
        case GLFW_KEY_F8: return InputKey::F8;
        case GLFW_KEY_F9: return InputKey::F9;
        case GLFW_KEY_F10: return InputKey::F10;
        case GLFW_KEY_F11: return InputKey::F11;
        case GLFW_KEY_F12: return InputKey::F12;
        case GLFW_KEY_F13: return InputKey::F13;
        case GLFW_KEY_F14: return InputKey::F14;
        case GLFW_KEY_F15: return InputKey::F15;
        case GLFW_KEY_F16: return InputKey::F16;
        case GLFW_KEY_F17: return InputKey::F17;
        case GLFW_KEY_F18: return InputKey::F18;
        case GLFW_KEY_F19: return InputKey::F19;
        case GLFW_KEY_F20: return InputKey::F20;
        case GLFW_KEY_F21: return InputKey::F21;
        case GLFW_KEY_F22: return InputKey::F22;
        case GLFW_KEY_F23: return InputKey::F23;
        case GLFW_KEY_F24: return InputKey::F24;
        case GLFW_KEY_F25: return InputKey::F25;
        case GLFW_KEY_KP_0: return InputKey::KP_0;
        case GLFW_KEY_KP_1: return InputKey::KP_1;
        case GLFW_KEY_KP_2: return InputKey::KP_2;
        case GLFW_KEY_KP_3: return InputKey::KP_3;
        case GLFW_KEY_KP_4: return InputKey::KP_4;
        case GLFW_KEY_KP_5: return InputKey::KP_5;
        case GLFW_KEY_KP_6: return InputKey::KP_6;
        case GLFW_KEY_KP_7: return InputKey::KP_7;
        case GLFW_KEY_KP_8: return InputKey::KP_8;
        case GLFW_KEY_KP_9: return InputKey::KP_9;
        case GLFW_KEY_KP_DECIMAL: return InputKey::KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return InputKey::KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return InputKey::KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return InputKey::KP_SUBTRACT;
        case GLFW_KEY_KP_ADD: return InputKey::KP_ADD;
        case GLFW_KEY_KP_ENTER: return InputKey::KP_ENTER;
        case GLFW_KEY_KP_EQUAL: return InputKey::KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return InputKey::LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return InputKey::LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT: return InputKey::LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER: return InputKey::LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return InputKey::RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return InputKey::RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT: return InputKey::RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER: return InputKey::RIGHT_SUPER;
        case GLFW_KEY_MENU: return InputKey::MENU;
        default: return InputKey::NONE;
        }
    }

    InputButton GLFWInput::TranslateButton(int button)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT: return InputButton::LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return InputButton::RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return InputButton::MIDDLE;
        case GLFW_MOUSE_BUTTON_4: return InputButton::EXTRA1;
        case GLFW_MOUSE_BUTTON_5: return InputButton::EXTRA2;
        case GLFW_MOUSE_BUTTON_6: return InputButton::EXTRA3;
        case GLFW_MOUSE_BUTTON_7: return InputButton::EXTRA4;
        case GLFW_MOUSE_BUTTON_8: return InputButton::EXTRA5;
        default: return InputButton::NONE;
        }
    }
}
