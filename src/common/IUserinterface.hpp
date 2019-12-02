#pragma once

namespace RIS
{
    class IUserinterface
    {
    public:
        virtual ~IUserinterface() = default;

        virtual void Draw() = 0;
        virtual void Update() = 0;
    };
}
