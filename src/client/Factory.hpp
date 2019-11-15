#pragma once

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/INetwork.hpp"
#include "common/IAudio.hpp"
#include "common/IUserinterface.hpp"

#include "common/SystemLocator.hpp"

#include <memory>
#include <string>

namespace RIS
{
    class SystemFactory
    {
    public:
        SystemFactory(SystemLocator &locator);
        ~SystemFactory();

        SystemFactory(const SystemFactory &) = delete;
        SystemFactory& operator= (const SystemFactory &) = delete;

        SystemFactory(SystemFactory &&) = delete;
        SystemFactory& operator= (SystemFactory &&) = delete;

        std::unique_ptr<IWindow> CreateWindow(const std::string &title) const;
        std::unique_ptr<IRenderer> CreateRenderer() const;
        std::unique_ptr<INetwork> CreateNetwork() const;
        std::unique_ptr<IAudio> CreateAudio() const;
        std::unique_ptr<IUserinterface> CreateUserinterface() const;

    private:
        SystemLocator &locator;

    };
}