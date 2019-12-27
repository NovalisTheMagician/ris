#pragma once

#include <string>

namespace RIS
{
    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual void SetRelativeMouse(bool setRelative) = 0;

        virtual void SetWindowIcon(const std::string &icon) = 0;
        virtual void SetCursorIcon(const std::string &cursor, int xHot, int yHot) = 0;

        virtual void Exit(int exitCode) = 0;

        virtual bool HandleMessages() = 0;
        virtual void Present() = 0;
    };
}