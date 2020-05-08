#pragma once

#include "SystemLocator.hpp"
#include "misc/Config.hpp"
#include "misc/Args.hpp"

namespace RIS
{
    const SystemLocator& GetSystems();
    Config& GetConfig();
    const Args& GetArgs();
}
