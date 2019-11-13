#pragma once

#include <stdexcept>
#include <string>

#include "IWindow.hpp"
#include "IRenderer.hpp"
#include "INetwork.hpp"
#include "IAudio.hpp"

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

        IWindow& GetWindow() const;
        IRenderer& GetRenderer() const;
        INetwork& GetNetwork() const;
        IAudio& GetAudio() const;

    private:
        IWindow *window;
        IRenderer *renderer;
        INetwork *network;
        IAudio *audio;

    };
}
