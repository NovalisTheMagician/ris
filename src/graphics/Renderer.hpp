#pragma once

#include "RisExcept.hpp"


namespace RIS::Graphics
{
    struct RendererException : public RISException
    {
        RendererException(const std::string &reason) : RISException(reason) {}
    };
    
    class Renderer
    {
    public:
        Renderer();
        ~Renderer() = default;
        Renderer(const Renderer &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator=(const Renderer &) = default;
        Renderer &operator=(Renderer &&) = default;

        void PostInit();

        void Resize(int width, int height);

    private:

    };
}
