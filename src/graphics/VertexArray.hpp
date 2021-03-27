#pragma once

#include "graphics/GLObject.hpp"

#include <glad/glad.h>

#include "graphics/Buffer.hpp"

namespace RIS::Graphics
{
    struct AttribFormat
    {
        int id;
        int numComponents;
        GLenum type;
        std::size_t offset;
        bool normalized;
        int bindingPoint;
    };

    class VertexArray : public GLObject
    {
    public:
        VertexArray();
        template<std::size_t NumAttribs>
        VertexArray(const AttribFormat (&attribs)[NumAttribs]);

        ~VertexArray();

        VertexArray(const VertexArray &) = delete;
        VertexArray& operator=(const VertexArray &) = delete;

        VertexArray(VertexArray &&other);
        VertexArray& operator=(VertexArray &&other);

        template<typename T, std::size_t Stride = sizeof(T)>
        void SetVertexBuffer(const Buffer &buffer, int bindingPoint = 0, std::size_t offset = 0);
        void SetIndexBuffer(const Buffer &buffer);

        template<std::size_t NumAttribs>
        void SetAttribFormat(const AttribFormat (&attribs)[NumAttribs]);
        void SetAttribFormat(int attrib, int numComponents, GLenum type, std::size_t offset, bool normalized = false, int bindingPoint = 0);

        void Bind();

    };

    template<std::size_t NumAttribs>
    VertexArray::VertexArray(const AttribFormat (&attribs)[NumAttribs])
        : VertexArray()
    {
        SetAttribFormat(attribs);
    }

    template<std::size_t NumAttribs>
    void VertexArray::SetAttribFormat(const AttribFormat (&attribs)[NumAttribs])
    {
        for(const AttribFormat &attrib : attribs)
        {
            SetAttribFormat(attrib.id, attrib.numComponents, attrib.type, attrib.offset, attrib.normalized, attrib.bindingPoint);
        }
    }

    template<typename T, std::size_t Stride>
    void VertexArray::SetVertexBuffer(const Buffer &buffer, int bindingPoint, std::size_t offset)
    {
        glVertexArrayVertexBuffer(id, bindingPoint, buffer.GetId(), offset, Stride);
    }
}
