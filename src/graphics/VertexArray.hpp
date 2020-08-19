#pragma once

#include "graphics/GLObject.hpp"

#include <glad/glad.h>

#include "graphics/Buffer.hpp"

namespace RIS
{
    namespace Graphics
    {
        class VertexArray : public GLObject
        {
        public:
            VertexArray();
            ~VertexArray();

            VertexArray(const VertexArray &) = delete;
            VertexArray& operator=(const VertexArray &) = delete;

            VertexArray(VertexArray &&other);
            VertexArray& operator=(VertexArray &&other);

            template<typename T, std::size_t Stride = sizeof T>
            void SetVertexBuffer(const Buffer &buffer, int bindingPoint = 0, std::size_t offset = 0);

            void SetIndexBuffer(const Buffer &buffer);
            void SetAttribFormat(int attrib, int numComponents, GLenum type, std::size_t offset, bool normalized = false, int bindingPoint = 0);

            void Bind();

        };

        template<typename T, std::size_t Stride>
        void VertexArray::SetVertexBuffer(const Buffer &buffer, int bindingPoint, std::size_t offset)
        {
            glVertexArrayVertexBuffer(id, bindingPoint, buffer.GetId(), offset, Stride);
        }
    }
}
