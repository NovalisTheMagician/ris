#include "Factory.hpp"

#include "GLFWWindow.hpp"
#include "GLRenderer.hpp"
#include "SOLAudio.hpp"
#include "SimpleUserinterface.hpp"
#include "common/BarcLoader.hpp"

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
    using IUserinterfacePtr = std::unique_ptr<IUserinterface>;
    using ILoaderPtr = std::unique_ptr<ILoader>;

    SystemFactory::SystemFactory(SystemLocator &locator) 
        : locator(locator)
    {

    }

    SystemFactory::~SystemFactory()
    {

    }

    IWindowPtr SystemFactory::CreateWindow(const string &title) const
    {
        auto system = std::make_unique<GLFWWindow>(locator, title);
        locator.Provide(system.get());
        return system;
    }

    IRendererPtr SystemFactory::CreateRenderer() const
    {
        auto system = std::make_unique<GLRenderer>(locator);
        locator.Provide(system.get());
        return system;
    }

    INetworkPtr SystemFactory::CreateNetwork() const
    {
#ifdef _WIN32
        auto system = std::make_unique<WinNetwork>(locator);
#elif defined __linux__
        auto system = std::make_unique<PosixNetwork>(locator);
#endif
        locator.Provide(system.get());
        return system;
    }

    IAudioPtr SystemFactory::CreateAudio() const
    {
        auto system = std::make_unique<SOLAudio>(locator);
        locator.Provide(system.get());
        return system;
    }

    IUserinterfacePtr SystemFactory::CreateUserinterface() const
    {
        auto system = std::make_unique<SimpleUserinterface>(locator);
        locator.Provide(system.get());
        return system;
    }

    ILoaderPtr SystemFactory::CreateLoader(const std::string &debugAssetFolder) const
    {
        auto system = std::make_unique<BarcLoader>(locator, debugAssetFolder);
        locator.Provide(system.get());
        return system;
    }
}
