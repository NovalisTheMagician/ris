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

        template<typename T>
        void SetVertexBuffer(const Buffer<T> &buffer, int bindingPoint);
        template<typename T>
        void SetIndexBuffer(const Buffer<T> &buffer);
        void SetAttribFormat(int attrib, int numComponents, gl::GLenum type, std::size_t offset, int bindingPoint = 0);

        void Bind();

    public:
        static VertexArray Create();

    };

    template<typename T>
    void VertexArray::SetVertexBuffer(const Buffer<T> &buffer, int bindingPoint)
    {
        gl::glVertexArrayVertexBuffer(id, bindingPoint, buffer.GetId(), 0, buffer.GetElementSize());
    }

    template<typename T>
    void VertexArray::SetIndexBuffer(const Buffer<T> &buffer)
    {
        gl::glVertexArrayElementBuffer(id, buffer.GetId());
    }
}
