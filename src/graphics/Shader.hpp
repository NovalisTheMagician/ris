#pragma once

#include "graphics/GLObject.hpp"

#include <glbinding/gl46core/gl.h>

#include <stdexcept>

namespace RIS
{
    namespace Graphics
    {
        enum class ShaderType
        {
            VERTEX,
            FRAGMENT
        };

        struct ShaderException : public std::runtime_error
        {
            ShaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
        };

        class Shader : public GLObject
        {
        public:
            Shader();
            virtual ~Shader();

            Shader(const Shader &) = delete;
            Shader& operator=(const Shader &) = delete;

            Shader(Shader &&other);
            Shader& operator=(Shader &&other);

            gl::UseProgramStageMask GetType() const;

        protected:
            gl::UseProgramStageMask type;

        };

        class BinaryShader : public Shader
        {
        public:
            BinaryShader(const std::vector<std::byte> &bytes, gl::GLenum shaderType);
        };

        class TextShader : public Shader
        {
        public:
            TextShader(const std::vector<std::byte> &bytes, gl::GLenum shaderType);
        };
    }
}
