#include "Factory.hpp"

using IWindowPtr = std::unique_ptr<RIS::IWindow>;
using IRendererPtr = std::unique_ptr<RIS::IRenderer>;
using INetworkPtr = std::unique_ptr<RIS::INetwork>;

namespace RIS
{
    IWindowPtr WindowFactory::Create()
    {
        return IWindowPtr(nullptr);
    }

    IRendererPtr RendererFactory::Create()
    {
        return IRendererPtr(nullptr);
    }

    INetworkPtr NetworkFactory::Create()
    {
        return INetworkPtr(nullptr);
    }
}
