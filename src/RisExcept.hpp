#pragma once

#include <string>
#include <stdexcept>

namespace RIS
{
    struct RISException : public std::runtime_error
    {
        RISException(const std::string &msg) : runtime_error(msg.c_str()) {};
    };
}
