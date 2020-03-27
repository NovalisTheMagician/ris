#include <memory>

#include <string>
#include <algorithm>

#include <glm/glm.hpp>

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/INetwork.hpp"
#include "common/IAudio.hpp"
#include "common/IUserinterface.hpp"
#include "common/ILoader.hpp"
#include "common/IInput.hpp"
#include "common/IScriptEngine.hpp"
#include "Factory.hpp"

#include "common/SystemLocator.hpp"
#include "common/Args.hpp"
#include "common/Config.hpp"

#include "common/Logger.hpp"

#include "common/Version.hpp"

#include "GameLoop.hpp"

using namespace std::literals::string_literals;

using namespace RIS;
using namespace RIS::Version;

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#undef CreateWindow
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int show)
{
    int argc = __argc;
    char **argv = __argv;
#else
int main(int argc, char *argv[])
{
#endif

    Logger &logger = Logger::Instance();
    logger.Info("Starting " + GAME_NAME + " Version " + std::to_string(MAJOR) + "." + std::to_string(MINOR));

    Args args(argc, argv);

    std::string configPath = "config.txt";
    if(args.IsSet("-config"))
        configPath = args.GetParameter("-config");
    Config config(configPath);

    std::string assetFolder = "";
    if(args.IsSet("-debug"))
    {
        if(args.IsSet("-assetpath"))
            assetFolder = args.GetParameter("-assetpath");
        else
            assetFolder = "assets";
    }

    logger.Info("Using config " + configPath);

    SystemLocator locator;

    const SystemFactory factory(locator, config);

    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<INetwork> network;
    std::unique_ptr<IAudio> audio;
    std::unique_ptr<IUserinterface> userinterface;
    std::unique_ptr<ILoader> loader;
    std::unique_ptr<IInput> input;
    std::unique_ptr<IScriptEngine> scriptEngine;

    try
    {
        window = factory.CreateWindow(GAME_NAME);
        renderer = factory.CreateRenderer();
        network = factory.CreateNetwork();
        audio = factory.CreateAudio();
        userinterface = factory.CreateUserinterface();
        loader = factory.CreateLoader(assetFolder);
        input = factory.CreateInput();
        scriptEngine = factory.CreateScriptEngine();

        std::string baseArchive = "test.zip";
        if(args.IsSet("-base"))
        {
            baseArchive = args.GetParameter("-base");
        }
        loader->AddOverlay(baseArchive);

        if(args.IsSet("-file"))
        {
            const auto &params = args.GetParameters("-base");
            std::for_each(params.begin(), params.end(), [&](const std::string &archiveFile)
            {
                loader->AddOverlay(archiveFile);
            });
        }

        renderer->PostInit();
        userinterface->PostInit();
        scriptEngine->PostInit();
    }
    catch(const std::exception& e)
    {
        logger.Error("Failed to init system: "s + e.what());
        Logger::Destroy();

#ifdef WIN32
        MessageBoxA(nullptr, e.what(), "Failed to launch game", MB_OK | MB_ICONERROR);
#endif

        return 1;
    }

    window->SetWindowIcon("cat_icon");
    window->SetCursorIcon("cat_cursor", 1, 4);

    logger.Info("System init OK");

    GameLoop loop(locator, config, args);
    int res = loop.Start();

    logger.Info("Exit game");

    Logger::Destroy();
    return res;
}
