#include "Factory.hpp"

#include "GLFWWindow.hpp"
#include "GLRenderer.hpp"
#include "SOLAudio.hpp"

#ifdef _WIN32
#include "common/WinNetwork.hpp"
#elif defined __linux__
#include "common/PosixNetwork.hpp"
#endif

#undef CreateWindow

using std::string;

namespace RIS
{
    using IWindowPtr = std::unique_ptr<IWindow>;
    using IRendererPtr = std::unique_ptr<IRenderer>;
    using INetworkPtr = std::unique_ptr<INetwork>;
    using IAudioPtr = std::unique_ptr<IAudio>;

    SystemFactory::SystemFactory(const SystemLocator &locator) 
        : locator(locator)
    {

    }

    SystemFactory::~SystemFactory()
    {

    }

    IWindowPtr SystemFactory::CreateWindow(const string &title) const
    {
        return std::make_unique<GLFWWindow>(locator, title);
    }

    IRendererPtr SystemFactory::CreateRenderer() const
    {
        return std::make_unique<GLRenderer>(locator);
    }

    INetworkPtr SystemFactory::CreateNetwork() const
    {
#ifdef _WIN32
        return std::make_unique<WinNetwork>(locator);
#elif defined __linux__
        return std::make_unique<PosixNetwork>(locator);
#endif
    }

    IAudioPtr SystemFactory::CreateAudio() const
    {
        return std::make_unique<SOLAudio>(locator);
    }
}
