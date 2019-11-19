#pragma once

namespace RIS
{
    class IWorld
    {
    public:
        virtual ~IWorld() = default;

        virtual int CreateActor() = 0;
    };
}
