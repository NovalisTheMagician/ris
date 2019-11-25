#pragma once

#include <stdexcept>
#include <string>

#include "IWindow.hpp"
#include "IRenderer.hpp"
#include "INetwork.hpp"
#include "IAudio.hpp"
#include "IUserinterface.hpp"
#include "ILoader.hpp"
#include "IPhysics.hpp"
#include "IWorld.hpp"
#include "IInput.hpp"

namespace RIS
{
    struct SystemNotProvidedException : public std::runtime_error
	{
		SystemNotProvidedException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class SystemLocator
    {
    public:
        SystemLocator();
        ~SystemLocator();

        SystemLocator(const SystemLocator &) = delete;
        SystemLocator & operator= (const SystemLocator &) = delete;

        SystemLocator(SystemLocator &&) = default;
        SystemLocator & operator= (SystemLocator &&) = default;

        void Provide(IWindow *window);
        void Provide(IRenderer *renderer);
        void Provide(INetwork *network);
        void Provide(IAudio *audio);
        void Provide(IUserinterface *userinterface);
        void Provide(ILoader *loader);
        void Provide(IPhysics *physics);
        void Provide(IWorld *world);
        void Provide(IInput *input);

        IWindow& GetWindow() const;
        IRenderer& GetRenderer() const;
        INetwork& GetNetwork() const;
        IAudio& GetAudio() const;
        IUserinterface& GetUserinterface() const;
        ILoader& GetLoader() const;
        IPhysics& GetPhysics() const;
        IWorld& GetWorld() const;
        IInput& GetInput() const;

    private:
        IWindow *window;
        IRenderer *renderer;
        INetwork *network;
        IAudio *audio;
        IUserinterface *userinterface;
        ILoader *loader;
        IPhysics *physics;
        IWorld *world;
        IInput *input;

    };
}
