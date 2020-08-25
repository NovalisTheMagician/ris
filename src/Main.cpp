#include <memory>

#include <string>
#include <algorithm>

#include "RisExcept.hpp"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

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
#include "script/ScriptEngine.hpp"

#include "graphics/Image.hpp"

#include "game/GameLoop.hpp"

#include "RIS.hpp"

#include <boxer/boxer.h>

using namespace std::literals::string_literals;

using namespace RIS;

namespace
{
    static Graphics::Renderer       *globalRenderer;
    static Window::Window           *globalWindow;
    static Audio::AudioEngine       *globalAudio;
    static Loader::Loader           *globalLoader;
    static Input::Input             *globalInput;
    static UI::Userinterface        *globalUserinterface;
    static Script::ScriptEngine     *globalScriptEngine;
    
    static Config globalConfig;
    static Args globalArgs;
}

#ifdef _WIN32
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

    ::globalConfig = std::move(config);
    ::globalArgs = args;

    logger.Info("Using config " + configPath);

    std::unique_ptr<Window::Window> window;
    std::unique_ptr<Graphics::Renderer> renderer;
    std::unique_ptr<Script::ScriptEngine> scriptEngine;
    std::unique_ptr<Audio::AudioEngine> audio;
    std::unique_ptr<UI::Userinterface> userinterface;
    std::unique_ptr<Loader::Loader> loader;
    std::unique_ptr<Input::Input> input;
    
    try
    {
        window = std::make_unique<Window::Window>(Version::GAME_NAME);
        renderer = std::make_unique<Graphics::Renderer>();
        scriptEngine = std::make_unique<Script::ScriptEngine>();
        audio = std::make_unique<Audio::AudioEngine>();
        userinterface = std::make_unique<UI::Userinterface>();
        loader = std::make_unique<Loader::Loader>(assetFolder);
        input = std::make_unique<Input::Input>(*window);

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
            const auto &params = args.GetParameters("-file");
            std::for_each(params.begin(), params.end(), [&](const std::string &archiveFile)
            {
                loader->AddOverlay(archiveFile);
            });
        }

        ::globalWindow = window.get();
        ::globalRenderer = renderer.get();
        ::globalLoader = loader.get();
        ::globalAudio = audio.get();
        ::globalInput = input.get();
        ::globalScriptEngine = scriptEngine.get();
        ::globalUserinterface = userinterface.get();

        loader->PostInit();
        window->PostInit();
        renderer->PostInit();
        audio->PostInit();
        userinterface->PostInit();
        scriptEngine->PostInit();

        scriptEngine->LoadScripts();
    }
    catch(const RISException& e)
    {
        logger.Error("Failed to init system: "s + e.what());
        Logger::Destroy();

        boxer::show(e.what(), "Failed to launch game", boxer::Style::Error);

        return 1;
    }

    auto windowIcon = loader->Load<Graphics::Image, false>("cat_icon.dds");
    auto windowCursor = loader->Load<Graphics::Image, false>("cat_cursor.dds");

    if(windowIcon)
        window->SetWindowIcon(windowIcon);
    if(windowCursor)
        window->SetCursorIcon(windowCursor, 1, 4);

    logger.Info("System init OK");

    Game::GameLoop loop;
    int res = 0;
    try
    {
        res = loop.Start();
    }
    catch(const RISException &e)
    {
        //logger.Error(e.what());

        boxer::show(e.what(), "Game Error", boxer::Style::Error);
    }

    logger.Info("Exit game");

    Logger::Destroy();
    return res;
}

namespace RIS
{
    Graphics::Renderer& GetRenderer()
    {
        return *::globalRenderer;
    }

    Loader::Loader& GetLoader()
    {
        return *::globalLoader;
    }

    UI::Userinterface& GetUserinterface()
    {
        return *::globalUserinterface;
    }

    UI::Console& GetConsole()
    {
        return ::globalUserinterface->GetConsole();
    }

    Audio::AudioEngine& GetAudioEngine()
    {
        return *::globalAudio;
    }

    Window::Window& GetWindow()
    {
        return *::globalWindow;
    }

    Input::Input& GetInput()
    {
        return *::globalInput;
    }

    Script::ScriptEngine& GetScriptEngine()
    {
        return *::globalScriptEngine;
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
