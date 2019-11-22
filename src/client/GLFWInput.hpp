#pragma once

#include "common/IInput.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include "GLFWWindow.hpp"

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
        const SystemLocator &systems;
        Config &config;

    };
}
