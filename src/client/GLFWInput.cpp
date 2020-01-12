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

    InputKeys GLFWInput::TranslateKey(int key)
    {
        switch(key)
        {
        case GLFW_KEY_SPACE: return InputKeys::SPACE;
        case GLFW_KEY_APOSTROPHE: return InputKeys::APOSTROPHE;
        case GLFW_KEY_COMMA: return InputKeys::COMMA;
        case GLFW_KEY_MINUS: return InputKeys::MINUS;
        case GLFW_KEY_PERIOD: return InputKeys::PERIOD;
        case GLFW_KEY_SLASH: return InputKeys::SLASH;
        case GLFW_KEY_0: return InputKeys::NUM_0;
        case GLFW_KEY_1: return InputKeys::NUM_1;
        case GLFW_KEY_2: return InputKeys::NUM_2;
        case GLFW_KEY_3: return InputKeys::NUM_3;
        case GLFW_KEY_4: return InputKeys::NUM_4;
        case GLFW_KEY_5: return InputKeys::NUM_5;
        case GLFW_KEY_6: return InputKeys::NUM_6;
        case GLFW_KEY_7: return InputKeys::NUM_7;
        case GLFW_KEY_8: return InputKeys::NUM_8;
        case GLFW_KEY_9: return InputKeys::NUM_9;
        case GLFW_KEY_SEMICOLON: return InputKeys::SEMICOLON;
        case GLFW_KEY_EQUAL: return InputKeys::EQUALS;
        case GLFW_KEY_A: return InputKeys::A;
        case GLFW_KEY_B: return InputKeys::B;
        case GLFW_KEY_C: return InputKeys::C;
        case GLFW_KEY_D: return InputKeys::D;
        case GLFW_KEY_E: return InputKeys::E;
        case GLFW_KEY_F: return InputKeys::F;
        case GLFW_KEY_G: return InputKeys::G;
        case GLFW_KEY_H: return InputKeys::H;
        case GLFW_KEY_I: return InputKeys::I;
        case GLFW_KEY_J: return InputKeys::J;
        case GLFW_KEY_K: return InputKeys::K;
        case GLFW_KEY_L: return InputKeys::L;
        case GLFW_KEY_M: return InputKeys::M;
        case GLFW_KEY_N: return InputKeys::N;
        case GLFW_KEY_O: return InputKeys::O;
        case GLFW_KEY_P: return InputKeys::P;
        case GLFW_KEY_Q: return InputKeys::Q;
        case GLFW_KEY_R: return InputKeys::R;
        case GLFW_KEY_S: return InputKeys::S;
        case GLFW_KEY_T: return InputKeys::T;
        case GLFW_KEY_U: return InputKeys::U;
        case GLFW_KEY_V: return InputKeys::V;
        case GLFW_KEY_W: return InputKeys::W;
        case GLFW_KEY_X: return InputKeys::X;
        case GLFW_KEY_Y: return InputKeys::Y;
        case GLFW_KEY_Z: return InputKeys::Z;
        case GLFW_KEY_LEFT_BRACKET: return InputKeys::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return InputKeys::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return InputKeys::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return InputKeys::GRAVE_ACCENT;
        case GLFW_KEY_WORLD_1: return InputKeys::WORLD_1;
        case GLFW_KEY_WORLD_2: return InputKeys::WORLD_2;
        case GLFW_KEY_ESCAPE: return InputKeys::ESCAPE;
        case GLFW_KEY_ENTER: return InputKeys::ENTER;
        case GLFW_KEY_TAB: return InputKeys::TAB;
        case GLFW_KEY_BACKSPACE: return InputKeys::BACKSPACE;
        case GLFW_KEY_INSERT: return InputKeys::INSERT;
        case GLFW_KEY_DELETE: return InputKeys::DELETE;
        case GLFW_KEY_RIGHT: return InputKeys::RIGHT;
        case GLFW_KEY_LEFT: return InputKeys::LEFT;
        case GLFW_KEY_DOWN: return InputKeys::DOWN;
        case GLFW_KEY_UP: return InputKeys::UP;
        case GLFW_KEY_PAGE_UP: return InputKeys::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return InputKeys::PAGE_DOWN;
        case GLFW_KEY_HOME: return InputKeys::HOME;
        case GLFW_KEY_END: return InputKeys::END;
        case GLFW_KEY_CAPS_LOCK: return InputKeys::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return InputKeys::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return InputKeys::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return InputKeys::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return InputKeys::PAUSE;
        case GLFW_KEY_F1: return InputKeys::F1;
        case GLFW_KEY_F2: return InputKeys::F2;
        case GLFW_KEY_F3: return InputKeys::F3;
        case GLFW_KEY_F4: return InputKeys::F4;
        case GLFW_KEY_F5: return InputKeys::F5;
        case GLFW_KEY_F6: return InputKeys::F6;
        case GLFW_KEY_F7: return InputKeys::F7;
        case GLFW_KEY_F8: return InputKeys::F8;
        case GLFW_KEY_F9: return InputKeys::F9;
        case GLFW_KEY_F10: return InputKeys::F10;
        case GLFW_KEY_F11: return InputKeys::F11;
        case GLFW_KEY_F12: return InputKeys::F12;
        case GLFW_KEY_F13: return InputKeys::F13;
        case GLFW_KEY_F14: return InputKeys::F14;
        case GLFW_KEY_F15: return InputKeys::F15;
        case GLFW_KEY_F16: return InputKeys::F16;
        case GLFW_KEY_F17: return InputKeys::F17;
        case GLFW_KEY_F18: return InputKeys::F18;
        case GLFW_KEY_F19: return InputKeys::F19;
        case GLFW_KEY_F20: return InputKeys::F20;
        case GLFW_KEY_F21: return InputKeys::F21;
        case GLFW_KEY_F22: return InputKeys::F22;
        case GLFW_KEY_F23: return InputKeys::F23;
        case GLFW_KEY_F24: return InputKeys::F24;
        case GLFW_KEY_F25: return InputKeys::F25;
        case GLFW_KEY_KP_0: return InputKeys::KP_0;
        case GLFW_KEY_KP_1: return InputKeys::KP_1;
        case GLFW_KEY_KP_2: return InputKeys::KP_2;
        case GLFW_KEY_KP_3: return InputKeys::KP_3;
        case GLFW_KEY_KP_4: return InputKeys::KP_4;
        case GLFW_KEY_KP_5: return InputKeys::KP_5;
        case GLFW_KEY_KP_6: return InputKeys::KP_6;
        case GLFW_KEY_KP_7: return InputKeys::KP_7;
        case GLFW_KEY_KP_8: return InputKeys::KP_8;
        case GLFW_KEY_KP_9: return InputKeys::KP_9;
        case GLFW_KEY_KP_DECIMAL: return InputKeys::KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return InputKeys::KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return InputKeys::KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return InputKeys::KP_SUBTRACT;
        case GLFW_KEY_KP_ADD: return InputKeys::KP_ADD;
        case GLFW_KEY_KP_ENTER: return InputKeys::KP_ENTER;
        case GLFW_KEY_KP_EQUAL: return InputKeys::KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return InputKeys::LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return InputKeys::LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT: return InputKeys::LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER: return InputKeys::LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return InputKeys::RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return InputKeys::RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT: return InputKeys::RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER: return InputKeys::RIGHT_SUPER;
        case GLFW_KEY_MENU: return InputKeys::MENU;
        default: return InputKeys::NONE;
        }
    }

    InputButtons GLFWInput::TranslateButton(int button)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT: return InputButtons::LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return InputButtons::RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return InputButtons::MIDDLE;
        case GLFW_MOUSE_BUTTON_4: return InputButtons::EXTRA1;
        case GLFW_MOUSE_BUTTON_5: return InputButtons::EXTRA2;
        case GLFW_MOUSE_BUTTON_6: return InputButtons::EXTRA3;
        case GLFW_MOUSE_BUTTON_7: return InputButtons::EXTRA4;
        case GLFW_MOUSE_BUTTON_8: return InputButtons::EXTRA5;
        default: return InputButtons::NONE;
        }
    }
}
