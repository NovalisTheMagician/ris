#pragma once

#include "graphics/GLObject.hpp"

#include <glad/glad.h>

#include <stdexcept>

#include <vector>

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

            GLenum GetType() const;

        protected:
            GLenum type;

        };

        class BinaryShader : public Shader
        {
        public:
            BinaryShader(const std::vector<std::byte> &bytes, GLenum shaderType);
        };

        class TextShader : public Shader
        {
        public:
            TextShader(const std::vector<std::byte> &bytes, GLenum shaderType);
        };
    }
}
