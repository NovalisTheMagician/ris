#pragma once

#include "RisExcept.hpp"

#include "graphics/GLObject.hpp"

#include <glad/glad.h>

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

        struct ShaderException : public RISException
        {
            ShaderException(const std::string &reason) : RISException(reason) {}
        };

        class Shader : public GLObject
        {
        public:
            Shader(const std::string &src, GLenum shaderType);
            virtual ~Shader();

            Shader(const Shader &) = delete;
            Shader& operator=(const Shader &) = delete;

            Shader(Shader &&other);
            Shader& operator=(Shader &&other);

            GLenum GetType() const;

        protected:
            GLenum type;

        };
    }
}
