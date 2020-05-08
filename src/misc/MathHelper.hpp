#pragma once

#include <math.h>

namespace RIS
{
    template<typename T>
    T Clamp(T min, T max, T val)
    {
        return val < min ? min : val > max ? max : val;
    }
}
