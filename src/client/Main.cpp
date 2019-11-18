#include <memory>

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

using std::unique_ptr;

using namespace RIS;

int main(int argc, char *argv[])
{
    Args args(argc, argv);
    Timer timer;

    SystemLocator locator;

    SystemFactory factory(locator);

    unique_ptr<IWindow> window = factory.CreateWindow("RIS");
    unique_ptr<IRenderer> renderer = factory.CreateRenderer();
    unique_ptr<INetwork> network = factory.CreateNetwork();
    unique_ptr<IAudio> audio = factory.CreateAudio();
    unique_ptr<IUserinterface> userinterface = factory.CreateUserinterface();
    unique_ptr<ILoader> loader = factory.CreateLoader("");

    glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);
    clearColor = glm::pow(clearColor, glm::vec4(2.2f));

    while (!window->HandleMessages())
    {
        renderer->Clear(clearColor);
        window->Present();
    }

    return 0;
}
