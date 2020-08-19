#pragma once

#include "graphics/GLObject.hpp"

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "graphics/Buffer.hpp"

#include <memory>

namespace RIS
{
    namespace Graphics
    {
        class Texture : public GLObject
        {
        public:
            using Ptr = std::shared_ptr<Texture>;

            Texture();
            Texture(const std::vector<std::byte> &data, bool flip = false);
            Texture(GLenum format, int width, int height);
            Texture(const glm::vec4 color);
            Texture(GLenum type);
            ~Texture();

            Texture(const Texture &) = delete;
            Texture& operator=(const Texture &) = delete;

            Texture(Texture &&other);
            Texture& operator=(Texture &&other);

            void SetBuffer(const Buffer &buffer, GLenum format);

            void Bind(GLuint textureUnit);

        };
    }
}
