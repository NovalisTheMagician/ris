#pragma once

#include <string>

namespace RIS
{
    class IUserinterface
    {
    public:
        virtual ~IUserinterface() = default;

        virtual void InitializeRootElements() = 0;

        virtual void LoadLayout(const std::string &layout) = 0;

        virtual void Draw() = 0;
        virtual void Update() = 0;
    };
}
