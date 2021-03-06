#pragma once

#include "RisExcept.hpp"

#include <string>
#include <sstream>
#include <functional>

#include <ctre.hpp>

namespace RIS::Graphics
{
    struct ShaderSourceBuilderException : public RISException
    {
        ShaderSourceBuilderException(const std::string &msg) : RISException(msg) {};
    };

    class ShaderSourceBuilder
    {
    public:
#ifdef __linux__
        std::string BuildSource(const std::string &src, std::function<std::string(std::string)> includeCallback, int level = 0)
#else
        template<typename T>
        std::string BuildSource(const std::string &src, T includeCallback, int level = 0)
#endif
        {
            if(level > MAX_INCLUDE_DEPTH)
            {
                throw ShaderSourceBuilderException("too many include recursions");
            }

            std::stringstream in(src);
            std::stringstream out;

            size_t lineNr = 1;

            std::string line;
            while(std::getline(in, line))
            {
                if(auto match = ctre::match<INCLUDE_PATTERN>(line))
                {
                    std::string file = match.get<1>().to_string();
                    std::string includeContent;
                    try
                    {
                        includeContent = includeCallback(file);
                    }
                    catch(const std::exception&)
                    {
                        throw ShaderSourceBuilderException("include file " + file + " not found");
                    }
                    out << BuildSource(includeContent, includeCallback, level+1) << "\n";
                    out << "#line " << (lineNr+1) << "\n";
                }
                else
                {
                    out << line << "\n";
                }
                ++lineNr;
            }

            return out.str();
        }

    private:
        static constexpr auto INCLUDE_PATTERN = ctll::fixed_string{ "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" };
        static const int MAX_INCLUDE_DEPTH = 32;

    };
}
