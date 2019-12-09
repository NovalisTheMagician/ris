#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    Shader::Shader()
        : programId(0)
    {
    }

    Shader::Shader(Shader &&other)
    {
        programId = other.programId;
        other.programId = 0;
    }

    Shader& Shader::operator=(Shader &&other)
    {
        programId = other.programId;
        other.programId = 0;
        return *this;
    }

    Shader::~Shader()
    {
        glDeleteProgram(programId);
    }

    void Shader::Create(const std::byte *shaderBinary, const std::size_t &size, gl::GLenum type)
    {
        if(programId != 0)
            glDeleteProgram(programId);

        programId = glCreateProgram();
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

        glProgramParameteri(programId, GL_PROGRAM_SEPARABLE, GL_TRUE);
        glAttachShader(programId, shader);
        glLinkProgram(programId);

        glGetProgramiv(programId, GL_LINK_STATUS, &status);
        if(!static_cast<bool>(status))
        {
            GLenum loglen;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(static_cast<int>(loglen), '#');
                glGetProgramInfoLog(shader, static_cast<GLsizei>(loglen), 0, &errorLog[0]);
                throw ShaderException("Failed to link shader: " + errorLog);
            }

            throw ShaderException("Failed to link shader");
        }

        glDetachShader(programId, shader);
        glDeleteShader(shader);
    }

    GLuint Shader::GetId() const
    {
        return programId;
    }
}
