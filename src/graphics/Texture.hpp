#pragma once

#include "graphics/GLObject.hpp"

#include <glm/glm.hpp>

#include <glad2/gl.h>

#include "graphics/Buffer.hpp"

#include <memory>

namespace RIS::Graphics
{
    enum class TextureFormat
    {
        R8 = GL_R8,
        R16 = GL_R16,
        RG8 = GL_RG8,
        RG16 = GL_RG16,
        R3_G3_B2 = GL_R3_G3_B2,
        RGB4 = GL_RGB4,
        RGB5 = GL_RGB5,
        RGB8 = GL_RGB8,
        RGB10 = GL_RGB10,
        RGB12 = GL_RGB12,
        RGB16 = GL_RGB16,
        RGB16_SNORM = GL_RGB16_SNORM,
        RGBA2 = GL_RGBA2,
        RGBA4 = GL_RGBA4,
        RGB5_A1 = GL_RGB5_A1,
        RGBA8 = GL_RGBA8,
        RGBA8_SNORM = GL_RGBA8_SNORM,
        RGB10_A2 = GL_RGB10_A2,
        RGB10_A2_UI = GL_RGB10_A2UI,
        RGBA12 = GL_RGBA12,
        RGBA16 = GL_RGBA16,

        DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
        DEPTH16 = GL_DEPTH_COMPONENT16,
        DEPTH24 = GL_DEPTH_COMPONENT24,
        DEPTH32 = GL_DEPTH_COMPONENT32,
        DEPTH32_F = GL_DEPTH_COMPONENT32F,

        R16_F = GL_R16F,
        RG16_F = GL_RG16F,
        RGB16_F = GL_RGB16F,
        RGBA16_F = GL_RGBA16F,
        R32_F = GL_R32F,
        RG32_F = GL_RG32F,
        RGB32_F = GL_RGB32F,
        RGBA32_F = GL_RGBA32F,
        RG11_B10_F = GL_R11F_G11F_B10F,

        R8_I = GL_R8I,
        R8_UI = GL_R8UI,
        R16_I = GL_R16I,
        R16_UI = GL_R16UI,
        R32_I = GL_R32I,
        R32_UI = GL_R32UI,
        RG8_I = GL_RG8I,
        RG8_UI = GL_RG8UI,
        RG16_I = GL_RG16I,
        RG16_UI = GL_RG16UI,
        RG32_I = GL_RG32I,
        RG32_UI = GL_RG32UI,
        RGB8_I = GL_RGB8I,
        RGB8_UI = GL_RGB8UI,
        RGB16_I = GL_RGB16I,
        RGB16_UI = GL_RGB16UI,
        RGB32_I = GL_RGB32I,
        RGB32_UI = GL_RGB32UI,
        RGBA8_I = GL_RGBA8I,
        RGBA8_UI = GL_RGBA8UI,
        RGBA16_I = GL_RGBA16I,
        RGBA16_UI = GL_RGBA16UI,
        RGBA32_I = GL_RGBA32I,
        RGBA32_UI = GL_RGBA32UI
    };

    class Texture : public GLObject
    {
    public:
        using Ptr = std::shared_ptr<Texture>;

        Texture();
        Texture(const std::vector<std::byte> &data, bool flip = false);
        Texture(const std::byte *rawData, int width, int height);
        Texture(TextureFormat format, int width, int height);
        Texture(const glm::vec4 color);
        Texture(GLenum type);
        ~Texture();

        Texture(const Texture &) = delete;
        Texture& operator=(const Texture &) = delete;

        Texture(Texture &&other);
        Texture& operator=(Texture &&other);

        void SetBuffer(const Buffer &buffer, TextureFormat format);

        void Bind(GLuint textureUnit) const;

    };
}
