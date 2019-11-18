#pragma once

#include <stdexcept>
#include <string>

#include "IWindow.hpp"
#include "IRenderer.hpp"
#include "INetwork.hpp"
#include "IAudio.hpp"
#include "IUserinterface.hpp"
#include "ILoader.hpp"

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

        IWindow& GetWindow() const;
        IRenderer& GetRenderer() const;
        INetwork& GetNetwork() const;
        IAudio& GetAudio() const;
        IUserinterface& GetUserinterface() const;
        ILoader& GetLoader() const;

    private:
        IWindow *window;
        IRenderer *renderer;
        INetwork *network;
        IAudio *audio;
        IUserinterface *userinterface;
        ILoader *loader;

    };
}
