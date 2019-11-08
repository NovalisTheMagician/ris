#pragma once

#include "common/INetwork.hpp"

#include <memory>

namespace RIS
{
    class NetworkFactory
    {
    public:
        static std::unique_ptr<INetwork> Create();
    };
}