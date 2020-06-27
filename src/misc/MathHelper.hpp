#pragma once

#include <math.h>

namespace RIS
{
    template<typename T>
    T Clamp(T min, T max, T val)
    {
        return val < min ? min : val > max ? max : val;
    }

    template <typename T> 
    int Signum(T val) 
    {
        return (T(0) < val) - (val < T(0));
    }
}
