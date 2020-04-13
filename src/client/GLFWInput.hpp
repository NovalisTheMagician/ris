#pragma once

#include "common/IInput.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include "GLFWWindow.hpp"

#include <unordered_map>

namespace RIS
{
    class GLFWInput : public IInput
    {
    public:
        GLFWInput(const SystemLocator &systems, Config &config, const GLFWWindow &window);
        ~GLFWInput();

        GLFWInput(const GLFWInput&) = delete;
        GLFWInput& operator=(const GLFWInput&) = delete;
        GLFWInput(GLFWInput&&) = delete;
        GLFWInput& operator=(GLFWInput&&) = delete;

        void RegisterKeyUp(const std::string &handle, KeyCallback callback) override;
        void RegisterKeyDown(const std::string &handle, KeyCallback callback) override;
        void RegisterButtonUp(const std::string &handle, ButtonCallback callback) override;
        void RegisterButtonDown(const std::string &handle, ButtonCallback callback) override;
        void RegisterMouse(const std::string &handle, MouseCallback callback) override;
        void RegisterWheel(const std::string &handle, WheelCallback callback) override;
        void RegisterChar(const std::string &handle, CharCallback callback) override;

        void UnregisterKeyUp(const std::string &handle) override;
        void UnregisterKeyDown(const std::string &handle) override;
        void UnregisterButtonUp(const std::string &handle) override;
        void UnregisterButtonDown(const std::string &handle) override;
        void UnregisterMouse(const std::string &handle) override;
        void UnregisterWheel(const std::string &handle) override;
        void UnregisterChar(const std::string &handle) override;

        void Update() override;

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

        InputKey TranslateKey(int key);
        InputButton TranslateButton(int button);

    private:
        static GLFWInput *instance;

    private:
        const SystemLocator &systems;
        Config &config;

        bool ready;

        std::unordered_map<std::string, RIS::KeyCallback> keyUpCallbacks;
        std::unordered_map<std::string, RIS::KeyCallback> keyDownCallbacks;

        std::unordered_map<std::string, RIS::ButtonCallback> buttonUpCallbacks;
        std::unordered_map<std::string, RIS::ButtonCallback> buttonDownCallbacks;

        std::unordered_map<std::string, RIS::MouseCallback> mouseCallbacks;
        std::unordered_map<std::string, RIS::WheelCallback> wheelCallbacks;

        std::unordered_map<std::string, RIS::CharCallback> charCallbacks;

    };
}
