#pragma once

#include "common/IRenderer.hpp"

#include "common/SystemLocator.hpp"

#include <stdexcept>

namespace RIS
{
    struct RendererException : public std::runtime_error
	{
		RendererException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class GLRenderer : public IRenderer
    {
    public:
        GLRenderer(const SystemLocator &systems);
        ~GLRenderer();

        GLRenderer(const GLRenderer &) = delete;
        GLRenderer(GLRenderer &&) = delete;
        GLRenderer &operator=(const GLRenderer &) = delete;
        GLRenderer &operator=(GLRenderer &&) = delete;

        void Clear(const glm::vec4 &clearColor) override;

    private:
        const SystemLocator &systems;

    };
}
