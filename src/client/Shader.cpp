#include "Shader.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    Shader::Shader()
        : GLObject(0)
    {
    }

    Shader::Shader(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type)
    {
        id = glCreateProgram();
        glProgramParameteri(id, GL_PROGRAM_SEPARABLE, GL_TRUE);
        
        GLuint shader = glCreateShader(type);
        glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary, size);
        glSpecializeShader(shader, "main", 0, nullptr, nullptr);

        GLenum status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(!static_cast<bool>(status))
        {
            GLenum loglen;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(static_cast<int>(loglen), '#');
                glGetShaderInfoLog(shader, static_cast<GLsizei>(loglen), 0, &errorLog[0]);
                throw ShaderException("Failed to load shader: " + errorLog);
            }

            throw ShaderException("Failed to load shader");
        }

        glAttachShader(id, shader);
        glLinkProgram(id);

        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if(!static_cast<bool>(status))
        {
            GLenum loglen;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(static_cast<int>(loglen), '#');
                glGetProgramInfoLog(id, static_cast<GLsizei>(loglen), 0, &errorLog[0]);
                throw ShaderException("Failed to link shader: " + errorLog);
            }

            throw ShaderException("Failed to link shader");
        }

        glDetachShader(id, shader);
        glDeleteShader(shader);

        switch(type)
        {
            case GL_VERTEX_SHADER: this->type = GL_VERTEX_SHADER_BIT; break;
            case GL_FRAGMENT_SHADER: this->type = GL_FRAGMENT_SHADER_BIT; break;
            case GL_GEOMETRY_SHADER: this->type = GL_GEOMETRY_SHADER_BIT; break;
            case GL_TESS_CONTROL_SHADER: this->type = GL_TESS_CONTROL_SHADER_BIT; break;
            case GL_TESS_EVALUATION_SHADER: this->type = GL_TESS_EVALUATION_SHADER_BIT; break;
            case GL_COMPUTE_SHADER: this->type = GL_COMPUTE_SHADER_BIT; break;
        }
    }

    Shader::Shader(Shader &&other)
    {
        std::swap(id, other.id);
        type = other.type;
    }

    Shader& Shader::operator=(Shader &&other)
    {
        std::swap(id, other.id);
        type = other.type;
        return *this;
    }

    Shader::~Shader()
    {
        glDeleteProgram(id);
    }

    Shader Shader::Create(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type)
    {
        return Shader(shaderBinary, size, type);
    }

    gl::UseProgramStageMask Shader::GetType() const
    {
        return type;
    }
}
