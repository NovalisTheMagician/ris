#pragma once

#include "GLObject.hpp"

#include <glbinding/gl46core/gl.h>

#include "Buffer.hpp"

namespace RIS
{
    class VertexArray : public GLObject
    {
    private:
        VertexArray(gl::GLuint id);

    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray &) = delete;
        VertexArray& operator=(const VertexArray &) = delete;

        VertexArray(VertexArray &&other);
        VertexArray& operator=(VertexArray &&other);

        void SetVertexBuffer(const Buffer &buffer, int bindingPoint = 0, size_t stride = 0, size_t offset = 0);
        template<typename T>
        void SetVertexBuffer(const Buffer &buffer, int bindingPoint = 0, size_t offset = 0);

        void SetIndexBuffer(const Buffer &buffer);
        void SetAttribFormat(int attrib, int numComponents, gl::GLenum type, std::size_t offset, int bindingPoint = 0);

        void Bind();

    public:
        static VertexArray Create();

    };

    template<typename T>
    void VertexArray::SetVertexBuffer(const Buffer &buffer, int bindingPoint, size_t offset)
    {
        SetVertexBuffer(buffer, bindingPoint, sizeof T, offset);
    }
}
