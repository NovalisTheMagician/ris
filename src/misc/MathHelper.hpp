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

    template<typename T>
    T Lerp(T a, T b, float t)
    {
        return a * (1.0f - t) + b * t;
    }

    template<typename T>
    class Bezier
    {
    public:
        T p1;
        T c1;
        T p2;
        T c2;
    };

    template<typename T>
    T Interpolate(const Bezier<T> &curve, float t)
    {
        return  curve.P1 * ((1 - t) * (1 - t) * (1 - t)) +
                curve.C1 * (3.0f * ((1 - t) * (1 - t)) * t) +
                curve.C2 * (3.0f * (1 - t) * (t * t)) +
                curve.P2 * (t * t * t);
    }
}
