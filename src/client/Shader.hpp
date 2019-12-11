#pragma once

#include "GLObject.hpp"

#include <glbinding/gl46core/gl.h>

#include <stdexcept>

namespace RIS
{
    struct ShaderException : public std::runtime_error
    {
        ShaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
    };

    class Shader : public GLObject
    {
    private:
        Shader(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type);

    public:
        Shader();
        ~Shader();

        Shader(const Shader &) = delete;
        Shader& operator=(const Shader &) = delete;

        Shader(Shader &&other);
        Shader& operator=(Shader &&other);

        gl::UseProgramStageMask GetType() const;

    public:
        static Shader Create(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type);

    private:
        gl::UseProgramStageMask type;

    };
}
