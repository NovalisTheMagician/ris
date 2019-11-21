#pragma once

#include "common/IUserinterface.hpp"
#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

namespace RIS
{
    class SimpleUserinterface : public IUserinterface
    {
    public:
        SimpleUserinterface(const SystemLocator &systems, Config &config);
        ~SimpleUserinterface();

        

    private:
        const SystemLocator &systems;
        Config &config;

    };
}