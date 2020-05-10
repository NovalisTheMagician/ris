#pragma once

#include "window/Window.hpp"

#include <unordered_map>
#include <functional>

#include "input/KeyDefs.hpp"

namespace RIS
{
    namespace Input
    {
        using KeyFunc = std::function<void(InputKey)>;
        using ButtonFunc = std::function<void(InputButton)>;
        using MouseFunc = std::function<void(float, float)>;
        using WheelFunc = std::function<void(float, float)>;
        using CharFunc = std::function<void(char)>;

        class Input
        {
        public:
            Input(const Window::Window &window);
            ~Input() = default;

            Input(const Input&) = delete;
            Input& operator=(const Input&) = delete;
            Input(Input&&) = default;
            Input& operator=(Input&&) = default;

            void RegisterKeyUp(const std::string &handle, KeyFunc callback);
            void RegisterKeyDown(const std::string &handle, KeyFunc callback);
            void RegisterButtonUp(const std::string &handle, ButtonFunc callback);
            void RegisterButtonDown(const std::string &handle, ButtonFunc callback);
            void RegisterMouse(const std::string &handle, MouseFunc callback);
            void RegisterWheel(const std::string &handle, WheelFunc callback);
            void RegisterChar(const std::string &handle, CharFunc callback);

            void UnregisterKeyUp(const std::string &handle);
            void UnregisterKeyDown(const std::string &handle);
            void UnregisterButtonUp(const std::string &handle);
            void UnregisterButtonDown(const std::string &handle);
            void UnregisterMouse(const std::string &handle);
            void UnregisterWheel(const std::string &handle);
            void UnregisterChar(const std::string &handle);

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

            std::unordered_map<std::string, KeyFunc> keyUpCallbacks;
            std::unordered_map<std::string, KeyFunc> keyDownCallbacks;

            std::unordered_map<std::string, ButtonFunc> buttonUpCallbacks;
            std::unordered_map<std::string, ButtonFunc> buttonDownCallbacks;

            std::unordered_map<std::string, MouseFunc> mouseCallbacks;
            std::unordered_map<std::string, WheelFunc> wheelCallbacks;

            std::unordered_map<std::string, CharFunc> charCallbacks;

        };
    }
}
