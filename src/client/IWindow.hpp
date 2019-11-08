#pragma once

namespace RIS
{
    class IWindow
    {
    public:
        virtual ~IWindow() {};

        virtual bool HandleMessages() = 0;
    };
}