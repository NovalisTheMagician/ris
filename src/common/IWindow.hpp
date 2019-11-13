#pragma once

namespace RIS
{
    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual bool HandleMessages() = 0;
        virtual void Present() = 0;
    };
}