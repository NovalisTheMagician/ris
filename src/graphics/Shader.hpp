#pragma once

#include "RisExcept.hpp"

#include "graphics/GLObject.hpp"

#include <glad2/gl.h>

#include <vector>
#include <memory>

namespace RIS::Graphics
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

    class Uniform;

    class Shader : public GLObject
    {
    public:
        using Ptr = std::shared_ptr<Shader>;

        Shader(const std::string &src, GLenum shaderType);
        virtual ~Shader();

        Shader(const Shader &) = delete;
        Shader& operator=(const Shader &) = delete;

        Shader(Shader &&other);
        Shader& operator=(Shader &&other);

        GLenum GetType() const;

        Uniform GetUniform(GLuint location) const;
        Uniform GetUniform(const std::string &name) const;

    protected:
        GLenum type;

    };

    class Uniform
    {
    public:
        Uniform(const Shader &shader, GLuint location);

        template<typename T>
        void Set(const T &value, bool transpose = false);

        template<typename T>
        void Set(const std::vector<T> &values, bool transpose = false);

    private:
        const Shader &shader;
        GLuint location;

    };
}
