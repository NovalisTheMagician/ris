#include <memory>

#include <string>

#include <glm/glm.hpp>

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/INetwork.hpp"
#include "common/IAudio.hpp"
#include "common/IUserinterface.hpp"
#include "common/ILoader.hpp"
#include "Factory.hpp"

#include "common/SystemLocator.hpp"
#include "common/Args.hpp"
#include "common/Timer.hpp"
#include "common/Config.hpp"

#include "common/Logger.hpp"

using std::unique_ptr;
using namespace std::literals::string_literals;

using namespace RIS;
#ifdef _WIN32
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
    logger.Info("Starting game...");

    Args args(argc, argv);
    Timer timer;
    Config config("config.txt");

    SystemLocator locator;

    const SystemFactory factory(locator, config);

    unique_ptr<IWindow> window;
    unique_ptr<IRenderer> renderer;
    unique_ptr<INetwork> network;
    unique_ptr<IAudio> audio;
    unique_ptr<IUserinterface> userinterface;
    unique_ptr<ILoader> loader;

    try
    {
        window = factory.CreateWindow("RIS");
        renderer = factory.CreateRenderer();
        network = factory.CreateNetwork();
        audio = factory.CreateAudio();
        userinterface = factory.CreateUserinterface();
        loader = factory.CreateLoader("assets");
    }
    catch(const std::exception& e)
    {
        logger.Error("Failed to init system: "s + e.what());
        Logger::Destroy();
        return 1;
    }

    logger.Info("System init OK");

    glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);
    clearColor = glm::pow(clearColor, glm::vec4(2.2f));

    while (!window->HandleMessages())
    {
        renderer->Clear(clearColor);
        window->Present();
    }

    logger.Info("Exit game");

    Logger::Destroy();
    return 0;
}
