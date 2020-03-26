#include "Factory.hpp"

#include "GLFWWindow.hpp"
#include "GLRenderer.hpp"
#include "SOLAudio.hpp"
#include "SimpleUserinterface.hpp"
#include "common/BarcLoader.hpp"
#include "common/ZipLoader.hpp"
#include "GLFWInput.hpp"
#include "common/LuaScriptEngine.hpp"

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
    using IInputPtr = std::unique_ptr<IInput>;
    using IScriptEnginePtr = std::unique_ptr<LuaScriptEngine>;

    SystemFactory::SystemFactory(SystemLocator &locator, Config &config) 
        : locator(locator), config(config)
    {

    }

    SystemFactory::~SystemFactory()
    {

    }

    IWindowPtr SystemFactory::CreateWindow(const string &title) const
    {
        auto system = std::make_unique<GLFWWindow>(locator, config, title);
        locator.Provide(system.get());
        return system;
    }

    IRendererPtr SystemFactory::CreateRenderer() const
    {
        auto system = std::make_unique<GLRenderer>(locator, config);
        locator.Provide(system.get());
        return system;
    }

    INetworkPtr SystemFactory::CreateNetwork() const
    {
#ifdef _WIN32
        auto system = std::make_unique<WinNetwork>(locator, config);
#elif defined __linux__
        auto system = std::make_unique<PosixNetwork>(locator, config);
#endif
        locator.Provide(system.get());
        return system;
    }

    IAudioPtr SystemFactory::CreateAudio() const
    {
        auto system = std::make_unique<SOLAudio>(locator, config);
        locator.Provide(system.get());
        return system;
    }

    IUserinterfacePtr SystemFactory::CreateUserinterface() const
    {
        auto system = std::make_unique<SimpleUserinterface>(locator, config);
        locator.Provide(system.get());
        return system;
    }

    ILoaderPtr SystemFactory::CreateLoader(const std::string &debugAssetFolder) const
    {
        auto system = std::make_unique<ZipLoader>(locator, config, debugAssetFolder);
        locator.Provide(system.get());
        return system;
    }

    IInputPtr SystemFactory::CreateInput() const
    {
        GLFWWindow *window = dynamic_cast<GLFWWindow*>(&locator.GetWindow());
        if(window == nullptr)
            throw std::runtime_error("Created window is incompatible with the input module");

        auto system = std::make_unique<GLFWInput>(locator, config, *window);
        locator.Provide(system.get());
        return system;
    }

    IScriptEnginePtr SystemFactory::CreateScriptEngine() const
    {
        auto system = std::make_unique<LuaScriptEngine>(locator, config);
        locator.Provide(system.get());
        return system;
    }
}
