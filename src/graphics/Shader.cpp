#include "graphics/Shader.hpp"

#include "misc/Logger.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RIS
{
    namespace Graphics
    {
        void CleanShader(std::string &source)
        {
            std::size_t nFPos;
            while((nFPos = source.find("#extension GL_GOOGLE_include_directive : enable")) != source.npos)
            {
                std::size_t secondNL = source.find('\n', nFPos);
                std::size_t firstNL = source.rfind('\n', nFPos);
                source.erase(firstNL, secondNL - firstNL);
            }

            while((nFPos = source.find("#line")) != source.npos)
            {
                std::size_t secondNL = source.find('\n', nFPos);
                std::size_t firstNL = source.rfind('\n', nFPos);
                source.erase(firstNL, secondNL - firstNL);
            }

            while((nFPos = source.find("#pragma")) != source.npos)
            {
                std::size_t secondNL = source.find('\n', nFPos);
                std::size_t firstNL = source.rfind('\n', nFPos);
                source.erase(firstNL, secondNL - firstNL);
            }
        }

        Shader::Shader()
            : GLObject(0)
        {}

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

        BinaryShader::BinaryShader(const std::vector<std::byte> &shaderBinary, GLenum type)
        {
            id = glCreateProgram();
            glProgramParameteri(id, GL_PROGRAM_SEPARABLE, GL_TRUE);
            
            GLuint shader = glCreateShader(type);
            glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary.data(), shaderBinary.size());
            glSpecializeShader(shader, "main", 0, nullptr, nullptr);

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

        TextShader::TextShader(const std::vector<std::byte> &shaderText, GLenum type)
        {
            id = glCreateProgram();
            glProgramParameteri(id, GL_PROGRAM_SEPARABLE, GL_TRUE);
            
            GLuint shader = glCreateShader(type);
            std::string shaderSrc(reinterpret_cast<const char*>(shaderText.data()), shaderText.size());

            CleanShader(shaderSrc);

            auto str = shaderSrc.c_str();

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
    }
}
