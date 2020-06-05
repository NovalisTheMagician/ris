#pragma once

#include "window/Window.hpp"

#include <vector>
#include <functional>

#include "input/KeyDefs.hpp"

namespace RIS
{
    namespace Input
    {
        using KeyFunc = std::function<bool(InputKey)>;
        using ButtonFunc = std::function<bool(InputButton)>;
        using MouseFunc = std::function<bool(float, float)>;
        using WheelFunc = std::function<bool(float, float)>;
        using CharFunc = std::function<bool(char)>;

        class Input
        {
        public:
            Input(const Window::Window &window);
            ~Input() = default;

            Input(const Input&) = delete;
            Input& operator=(const Input&) = delete;
            Input(Input&&) = default;
            Input& operator=(Input&&) = default;

            void RegisterKeyUp(KeyFunc callback, bool insertLast = false);
            void RegisterKeyDown(KeyFunc callback, bool insertLast = false);
            void RegisterButtonUp(ButtonFunc callback, bool insertLast = false);
            void RegisterButtonDown(ButtonFunc callback, bool insertLast = false);
            void RegisterMouse(MouseFunc callback, bool insertLast = false);
            void RegisterWheel(WheelFunc callback, bool insertLast = false);
            void RegisterChar(CharFunc callback, bool insertLast = false);

            void Update();

        private:
            static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
            static void CharCallback(GLFWwindow* window, unsigned int codepoint);
            static void MousePosCallback(GLFWwindow* window, double x, double y);
            static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
            static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

            void OnKeyUp(int key);
            void OnKeyDown(int key);
            void OnChar(char character);
            void OnMouseMove(float x, float y);
            void OnScrollWheel(float x, float y);
            void OnMouseButtonUp(int button);
            void OnMouseButtonDown(int button);

            constexpr InputKey TranslateKey(int key);
            constexpr InputButton TranslateButton(int button);

        private:
            static Input *instance;

        private:
            bool ready = false;

            std::vector<KeyFunc> keyUpCallbacks;
            std::vector<KeyFunc> keyDownCallbacks;

            std::vector<ButtonFunc> buttonUpCallbacks;
            std::vector<ButtonFunc> buttonDownCallbacks;

            std::vector<MouseFunc> mouseCallbacks;
            std::vector<WheelFunc> wheelCallbacks;

            std::vector<CharFunc> charCallbacks;

        };
    }
}
