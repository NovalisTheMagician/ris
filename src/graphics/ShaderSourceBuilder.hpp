#pragma once

#include <string>
#include <sstream>
#include <functional>
#include <stdexcept>

#include <ctre.hpp>

namespace RIS
{
    namespace Graphics
    {
        class ShaderSourceBuilder
        {
        public:
            template<typename T>
            std::string BuildSource(const std::string &src, const std::string &fileName, T includeCallback, int level = 0);

        private:
            static constexpr auto INCLUDE_PATTERN = ctll::fixed_string{ "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" };
            static const int MAX_INCLUDE_DEPTH = 32;

        };

        template<typename T>
        std::string ShaderSourceBuilder::BuildSource(const std::string &src, const std::string &fileName, T includeCallback, int level)
        {
            if(level > MAX_INCLUDE_DEPTH)
            {
                throw std::runtime_error("too many include recursions");
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
                    catch(const std::exception& e)
                    {
                        throw std::runtime_error("include file " + file + " not found");
                    }
                    out << BuildSource(includeContent, file, includeCallback, level+1) << "\n";
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
    }
}
