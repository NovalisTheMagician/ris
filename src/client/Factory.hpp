#pragma once

#include "IWindow.hpp"
#include "IRenderer.hpp"
#include "common/INetwork.hpp"

#include <memory>

namespace RIS
{
    class WindowFactory
    {
    public:
        static std::unique_ptr<IWindow> Create();
    };

    class RendererFactory
    {
    public:
        static std::unique_ptr<IRenderer> Create();
    };

    class NetworkFactory
    {
    public:
        static std::unique_ptr<INetwork> Create();
    };
}