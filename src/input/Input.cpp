#pragma once

#include "input/Input.hpp"

using std::string;

namespace RIS
{
    namespace Input
    {
        Input *Input::instance = nullptr;

        Input::Input(const Window::Window &window)
        {
            GLFWwindow *wnd = window.GetWindowHandle();

            glfwSetKeyCallback(wnd, KeyCallback);
            glfwSetCharCallback(wnd, CharCallback);
            glfwSetCursorPosCallback(wnd, MousePosCallback);
            glfwSetScrollCallback(wnd, MouseScrollCallback);
            glfwSetMouseButtonCallback(wnd, MouseButtonCallback);
            ready = true;
            instance = this;
        }

        void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {  
            Input &input = *instance;
            if(action == GLFW_PRESS)
            {
                input.OnKeyDown(key);
            }
            else if(action == GLFW_RELEASE)
            {
                input.OnKeyUp(key);
            }
        }

        void Input::CharCallback(GLFWwindow *window, uint32_t codePoint)
        {
            Input &input = *instance;
            input.OnChar(codePoint);
        }

        void Input::MousePosCallback(GLFWwindow* window, double x, double y)
        {
            Input &input = *instance;
            input.OnMouseMove(static_cast<float>(x), static_cast<float>(y));
        }

        void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            Input &input = *instance;
            if(action == GLFW_PRESS)
            {
                input.OnMouseButtonDown(button);
            }
            else if(action == GLFW_RELEASE)
            {
                input.OnMouseButtonUp(button);
            }
        }
        
        void Input::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            Input &input = *instance;
            input.OnScrollWheel(static_cast<float>(xoffset), static_cast<float>(yoffset));
        }

        void Input::OnKeyUp(int key)
        {
            if(!ready) return;
            for(auto& callback : keyUpCallbacks)
                if(callback(TranslateKey(key)))
                    break;
        }

        void Input::OnKeyDown(int key)
        {
            if(!ready) return;
            for(auto& callback : keyDownCallbacks)
                if(callback(TranslateKey(key)))
                    break;
        }

        void Input::OnChar(uint32_t character)
        {
            if(!ready) return;
            for(auto& callback : charCallbacks)
                if(callback(character))
                    break;
        }

        void Input::OnMouseMove(float x, float y)
        {
            if(!ready) return;
            for(auto& callback : mouseCallbacks)
                if(callback(x, y))
                    break;
        }

        void Input::OnScrollWheel(float x, float y)
        {
            if(!ready) return;
            for(auto& callback : wheelCallbacks)
                if(callback(x, y))
                    break;
        }

        void Input::OnMouseButtonUp(int button)
        {
            if(!ready) return;
            for(auto& callback : buttonUpCallbacks)
                if(callback(TranslateKey(button)))
                    break;
        }

        void Input::OnMouseButtonDown(int button)
        {
            if(!ready) return;
            for(auto& callback : buttonDownCallbacks)
                if(callback(TranslateKey(button)))
                    break;
        }

        void Input::RegisterKeyUp(KeyFunc callback, bool insertLast)
        {
            auto insertPoint = insertLast ? keyUpCallbacks.end() : keyUpCallbacks.begin();
            keyUpCallbacks.insert(insertPoint, callback);
        }

        void Input::RegisterKeyDown(KeyFunc callback, bool insertLast)
        {
            auto insertPoint = insertLast ? keyDownCallbacks.end() : keyDownCallbacks.begin();
            keyDownCallbacks.insert(insertPoint, callback);
        }

        void Input::RegisterButtonUp(ButtonFunc callback, bool insertLast) 
        {
            auto insertPoint = insertLast ? buttonUpCallbacks.end() : buttonUpCallbacks.begin();
            buttonUpCallbacks.insert(insertPoint, callback);
        }

        void Input::RegisterButtonDown(ButtonFunc callback, bool insertLast)
        {
            auto insertPoint = insertLast ? buttonDownCallbacks.end() : buttonDownCallbacks.begin();
            buttonDownCallbacks.insert(insertPoint, callback);
        }

        void Input::RegisterMouse(MouseFunc callback, bool insertLast)
        {
            auto insertPoint = insertLast ? mouseCallbacks.end() : mouseCallbacks.begin();
            mouseCallbacks.insert(insertPoint, callback);
        }

        void Input::RegisterWheel(WheelFunc callback, bool insertLast)
        {
            auto insertPoint = insertLast ? wheelCallbacks.end() : wheelCallbacks.begin();
            wheelCallbacks.insert(insertPoint, callback);
        }

        void Input::RegisterChar(CharFunc callback, bool insertLast)
        {
            auto insertPoint = insertLast ? charCallbacks.end() : charCallbacks.begin();
            charCallbacks.insert(insertPoint, callback);
        }

        void Input::Update()
        {

        }

        constexpr InputKey Input::TranslateKey(int key)
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
            case GLFW_MOUSE_BUTTON_LEFT: return InputKey::MOUSE_LEFT;
            case GLFW_MOUSE_BUTTON_RIGHT: return InputKey::MOUSE_RIGHT;
            case GLFW_MOUSE_BUTTON_MIDDLE: return InputKey::MOUSE_MIDDLE;
            case GLFW_MOUSE_BUTTON_4: return InputKey::MOUSE_EXTRA1;
            case GLFW_MOUSE_BUTTON_5: return InputKey::MOUSE_EXTRA2;
            case GLFW_MOUSE_BUTTON_6: return InputKey::MOUSE_EXTRA3;
            case GLFW_MOUSE_BUTTON_7: return InputKey::MOUSE_EXTRA4;
            case GLFW_MOUSE_BUTTON_8: return InputKey::MOUSE_EXTRA5;
            default: return InputKey::NONE;
            }
        }
    }
}
