#include "SystemLocator.hpp"

namespace RIS
{
    SystemLocator::SystemLocator() 
        : window(nullptr), renderer(nullptr), network(nullptr), audio(nullptr), userinterface(nullptr), loader(nullptr),
        physics(nullptr), world(nullptr)
    {

    }

    SystemLocator::~SystemLocator() 
    {

    }

    void SystemLocator::Provide(IWindow *window)
    {
        this->window = window;
    }

    void SystemLocator::Provide(IRenderer *renderer)
    {
        this->renderer = renderer;
    }

    void SystemLocator::Provide(INetwork *network)
    {
        this->network = network;
    }

    void SystemLocator::Provide(IAudio *audio)
    {
        this->audio = audio;
    }

    void SystemLocator::Provide(IUserinterface *userinterface)
    {
        this->userinterface = userinterface;
    }

    void SystemLocator::Provide(ILoader *loader)
    {
        this->loader = loader;
    }

    void SystemLocator::Provide(IPhysics *physics)
    {
        this->physics = physics;
    }

    void SystemLocator::Provide(IWorld *world)
    {
        this->world = world;
    }

    IWindow& SystemLocator::GetWindow() const
    {
        if(window == nullptr)
            throw SystemNotProvidedException("Window");

        return *window;
    }

    IRenderer& SystemLocator::GetRenderer() const
    {
        if(renderer == nullptr)
            throw SystemNotProvidedException("Renderer");
            
        return *renderer;
    }

    INetwork& SystemLocator::GetNetwork() const
    {
        if(network == nullptr)
            throw SystemNotProvidedException("Network");
            
        return *network;
    }

    IAudio& SystemLocator::GetAudio() const
    {
        if(audio == nullptr)
            throw SystemNotProvidedException("Audio");
            
        return *audio;
    }

    IUserinterface& SystemLocator::GetUserinterface() const
    {
        if(userinterface == nullptr)
            throw SystemNotProvidedException("Userinterface");
            
        return *userinterface;
    }

    ILoader& SystemLocator::GetLoader() const
    {
        if(loader == nullptr)
            throw SystemNotProvidedException("Loader");
            
        return *loader;
    }

    IPhysics& SystemLocator::GetPhysics() const
    {
        if(physics == nullptr)
            throw SystemNotProvidedException("Physics");
            
        return *physics;
    }

    IWorld& SystemLocator::GetWorld() const
    {
        if(world == nullptr)
            throw SystemNotProvidedException("World");
            
        return *world;
    }
}
