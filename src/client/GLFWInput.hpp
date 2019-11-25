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

    private:
        static const std::unordered_map<int, InputAction> DEFAULT_KEYMAP;
        static GLFWInput *instance;

    private:
        const SystemLocator &systems;
        Config &config;

        std::unordered_map<int, InputAction> keyMap;

    };
}
