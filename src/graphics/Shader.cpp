#include "graphics/Shader.hpp"

#include "misc/Logger.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RIS::Graphics
{
    Shader::Shader(const std::string &src, GLenum shaderType)
        : GLObject(0)
    {
        id = glCreateProgram();
        glProgramParameteri(id, GL_PROGRAM_SEPARABLE, GL_TRUE);
        
        GLuint shader = glCreateShader(shaderType);
        auto str = src.c_str();

        glShaderSource(shader, 1, &str, nullptr);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(!status)
        {
            GLsizei loglen;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(loglen, '#');
                glGetShaderInfoLog(shader, loglen, 0, &errorLog[0]);
                throw ShaderException("Failed to load shader: " + errorLog);
            }

            throw ShaderException("Failed to load shader");
        }

        glAttachShader(id, shader);
        glLinkProgram(id);

        GLint isSeperable = GL_TRUE;
        glGetProgramiv(id, GL_PROGRAM_SEPARABLE, &isSeperable);
        if(!isSeperable)
        {
            throw ShaderException("Seperate Shader Objects not supported!!!");
        }

        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if(!status)
        {
            GLsizei loglen;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglen);
            if(loglen != 0)
            {
                std::string errorLog(loglen, '#');
                glGetProgramInfoLog(id, loglen, 0, &errorLog[0]);
                throw ShaderException("Failed to link shader: " + errorLog);
            }

            throw ShaderException("Failed to link shader");
        }

        glDetachShader(id, shader);
        glDeleteShader(shader);

        switch(shaderType)
        {
            case GL_VERTEX_SHADER: type = GL_VERTEX_SHADER_BIT; break;
            case GL_FRAGMENT_SHADER: type = GL_FRAGMENT_SHADER_BIT; break;
            case GL_GEOMETRY_SHADER: type = GL_GEOMETRY_SHADER_BIT; break;
            case GL_TESS_CONTROL_SHADER: type = GL_TESS_CONTROL_SHADER_BIT; break;
            case GL_TESS_EVALUATION_SHADER: type = GL_TESS_EVALUATION_SHADER_BIT; break;
            case GL_COMPUTE_SHADER: type = GL_COMPUTE_SHADER_BIT; break;
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

    GLenum Shader::GetType() const
    {
        return type;
    }

    Uniform Shader::GetUniform(GLuint location) const
    {
        return Uniform(*this, location);
    }

    Uniform Shader::GetUniform(const std::string &name) const
    {
        GLint location = glGetUniformLocation(id, name.c_str());
        if(location == -1)
            throw ShaderException("uniform (" + name + ") not found");
        return Uniform(*this, location);
    }

    Uniform::Uniform(const Shader &shader, GLuint location)
        : shader(shader), location(location)
    {}

    template<>
    void Uniform::Set<float>(const float &value, bool transpose)
    {
        glProgramUniform1f(shader.GetId(), location, value);
    }

    template<>
    void Uniform::Set<int>(const int &value, bool transpose)
    {
        glProgramUniform1i(shader.GetId(), location, value);
    }

    template<>
    void Uniform::Set<unsigned int>(const unsigned int &value, bool transpose)
    {
        glProgramUniform1ui(shader.GetId(), location, value);
    }

    template<>
    void Uniform::Set<glm::vec2>(const glm::vec2 &value, bool transpose)
    {
        glProgramUniform2fv(shader.GetId(), location, 2, glm::value_ptr(value));
    }

    template<>
    void Uniform::Set<glm::vec3>(const glm::vec3 &value, bool transpose)
    {
        glProgramUniform2fv(shader.GetId(), location, 3, glm::value_ptr(value));
    }

    template<>
    void Uniform::Set<glm::vec4>(const glm::vec4 &value, bool transpose)
    {
        glProgramUniform2fv(shader.GetId(), location, 4, glm::value_ptr(value));
    }

    template<>
    void Uniform::Set<glm::mat4>(const glm::mat4 &value, bool transpose)
    {
        glProgramUniformMatrix4fv(shader.GetId(), location, 1, transpose, glm::value_ptr(value));
    }

    template<>
    void Uniform::Set<float>(const std::vector<float> &values, bool transpose)
    {
        glProgramUniform1fv(shader.GetId(), location, values.size(), values.data());
    }

    template<>
    void Uniform::Set<int>(const std::vector<int> &values, bool transpose)
    {
        glProgramUniform1iv(shader.GetId(), location, values.size(), values.data());
    }

    template<>
    void Uniform::Set<unsigned int>(const std::vector<unsigned int> &values, bool transpose)
    {
        glProgramUniform1uiv(shader.GetId(), location, values.size(), values.data());
    }

    template<>
    void Uniform::Set<glm::vec2>(const std::vector<glm::vec2> &values, bool transpose)
    {
        glProgramUniform2fv(shader.GetId(), location, values.size(), glm::value_ptr(values[0]));
    }

    template<>
    void Uniform::Set<glm::vec3>(const std::vector<glm::vec3> &values, bool transpose)
    {
        glProgramUniform3fv(shader.GetId(), location, values.size(), glm::value_ptr(values[0]));
    }

    template<>
    void Uniform::Set<glm::vec4>(const std::vector<glm::vec4> &values, bool transpose)
    {
        glProgramUniform4fv(shader.GetId(), location, values.size(), glm::value_ptr(values[0]));
    }

    template<>
    void Uniform::Set<glm::mat4>(const std::vector<glm::mat4> &values, bool transpose)
    {
        glProgramUniformMatrix4fv(shader.GetId(), location, values.size(), transpose, glm::value_ptr(values[0]));
    }
}
