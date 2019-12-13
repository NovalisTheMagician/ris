#include "Texture.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gli/gli.hpp>

using namespace gl46core;

namespace RIS
{
    Texture::Texture()
        : GLObject(0)
    {
    }

    Texture::Texture(const std::byte *data, const std::size_t &size, bool flip)
    {
        gli::texture texture = gli::load(reinterpret_cast<const char*>(data), size);
        if(texture.empty())
            return;
        
        if(flip)
            texture = gli::flip(texture);

        gli::gl gl(gli::gl::PROFILE_GL33);
        gli::gl::format const format = gl.translate(texture.format(), texture.swizzles());
        glm::tvec3<GLsizei> extent(texture.extent(0));
        GLenum target = static_cast<GLenum>(gl.translate(texture.target()));

        GLsizei const faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());

        glCreateTextures(target, 1, &id);
        glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);
        glTextureParameteri(id, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
        glTextureParameteriv(id, GL_TEXTURE_SWIZZLE_RGBA, &format.Swizzles[0]);

        bool isCube = texture.target() == gli::TARGET_CUBE;

        switch(texture.target())
        {
            case gli::TARGET_2D: 
            case gli::TARGET_CUBE:
                glTextureStorage2D(id, static_cast<GLint>(texture.levels()), static_cast<GLenum>(format.Internal), extent.x, isCube ? faceTotal : extent.y);
                break;
            default:
                return;
        }

        for(std::size_t face = 0; face < texture.faces(); ++face)
        for(std::size_t level = 0; level < texture.levels(); ++level)
        {
            extent = glm::tvec3<GLsizei>(texture.extent(level));
            target = gli::is_target_cube(texture.target()) ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : target;

            if(gli::is_compressed(texture.format()))
                glCompressedTextureSubImage2D(id, static_cast<GLint>(level), 0, 0, extent.x, extent.y, static_cast<GLenum>(format.Internal), static_cast<GLsizei>(texture.size(level)), texture.data(0, face, level));
            else
                glTextureSubImage2D(id, static_cast<GLint>(level), 0, 0, extent.x, extent.y, static_cast<GLenum>(format.External), static_cast<GLenum>(format.Type), texture.data(0, face, level));
        }
        
        if(texture.levels() == 1)
        {
            glGenerateTextureMipmap(id);
        }
    }

    Texture::Texture(gl::GLenum format, int width, int height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        glTextureStorage2D(id, 1, format, width, height);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Texture::Texture(const glm::vec4 color)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        glTextureStorage2D(id, 1, GL_RGBA8, 1, 1);
        glTextureSubImage2D(id, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, glm::value_ptr(color));
    }

    Texture::Texture(gl::GLenum type)
    {
        glCreateTextures(type, 1, &id);
    }

    Texture::Texture(Texture &&other)
    {
        std::swap(id, other.id);
    }

    Texture& Texture::operator=(Texture &&other)
    {
        std::swap(id, other.id);
        return *this;
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    Texture Texture::Create(const std::byte *data, const std::size_t &size, bool flip)
    {
        return Texture(data, size, flip);
    }

    Texture Texture::Create(gl::GLenum format, int width, int height)
    {
        return Texture(format, width, height);
    }

    Texture Texture::Create(const glm::vec4 color)
    {
        return Texture(color);
    }

    Texture Texture::Create()
    {
        return Texture(GL_TEXTURE_BUFFER);
    }

    void Texture::Bind(GLuint textureUnit)
    {
        glBindTextureUnit(textureUnit, id);
    }
}
