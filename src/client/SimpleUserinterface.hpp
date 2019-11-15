#pragma once

#include "common/IUserinterface.hpp"
#include "common/SystemLocator.hpp"

namespace RIS
{
    class SimpleUserinterface : public IUserinterface
    {
    public:
        SimpleUserinterface(const SystemLocator &systems);
        ~SimpleUserinterface();

        

    private:
        const SystemLocator &systems;

    };
}