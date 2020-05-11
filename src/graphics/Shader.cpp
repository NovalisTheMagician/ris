#include "graphics/Shader.hpp"

#include "misc/Logger.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RIS
{
    namespace Graphics
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
    }
}
