#include <memory>

#include <string>
#include <algorithm>

#include "Factory.hpp"

#include "misc/Args.hpp"
#include "misc/Config.hpp"
#include "misc/Logger.hpp"
#include "misc/Version.hpp"

#include "game/GameLoop.hpp"

#include "graphics/Renderer.hpp"
#include "window/Window.hpp"
#include "audio/AudioEngine.hpp"
#include "ui/Userinterface.hpp"
#include "input/Input.hpp"
#include "loader/Loader.hpp"

#include "graphics/Image.hpp"

#include "game/GameLoop.hpp"

#include "RIS.hpp"

using namespace std::literals::string_literals;

using namespace RIS;

namespace
{
    static SystemLocator globalSystemLocator;
    static Config globalConfig;
    static Args globalArgs;
}

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
    logger.Info("Starting " + Version::GAME_NAME + " Version " + std::to_string(Version::MAJOR) + "." + std::to_string(Version::MINOR));

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

    const SystemFactory factory(locator);

    std::unique_ptr<Window::Window> window;
    std::unique_ptr<Graphics::Renderer> renderer;
    std::unique_ptr<Audio::AudioEngine> audio;
    std::unique_ptr<UI::Userinterface> userinterface;
    std::unique_ptr<Loader::Loader> loader;
    std::unique_ptr<Input::Input> input;
    std::unique_ptr<Script::ScriptEngine> scriptEngine;

    try
    {
        window = factory.CreateWindow(Version::GAME_NAME);
        renderer = factory.CreateRenderer();
        audio = factory.CreateAudio();
        userinterface = factory.CreateUserinterface();
        loader = factory.CreateLoader(assetFolder);
        input = factory.CreateInput();
        scriptEngine = factory.CreateScriptEngine();

        std::string baseArchive = "main.zip";
        if(args.IsSet("-debug"))
        {
            baseArchive = "";
        }
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

        ::globalSystemLocator = std::move(locator);
        ::globalConfig = std::move(config);
        ::globalArgs = std::move(args);

        window->PostInit();
        renderer->PostInit();
        audio->PostInit();
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

    try
    {
        auto windowIcon = loader->Load<Graphics::Image>("cat_icon.dds");
        auto windowCursor = loader->Load<Graphics::Image>("cat_cursor.dds");

        window->SetWindowIcon(windowIcon);
        window->SetCursorIcon(windowCursor, 1, 4);
    }
    catch(const std::runtime_error &e)
    {

    }

    logger.Info("System init OK");

    Game::GameLoop loop;
    int res = loop.Start();

    logger.Info("Exit game");

    Logger::Destroy();
    return res;
}

namespace RIS
{
    const SystemLocator& GetSystems()
    {
        return ::globalSystemLocator;
    }

    Config& GetConfig()
    {
        return ::globalConfig;
    }

    const Args& GetArgs()
    {
        return ::globalArgs;
    }
}
