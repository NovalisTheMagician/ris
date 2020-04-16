#pragma once

//#include "GLRenderer.hpp"
#include "Buffer.hpp"
#include "VertexTypes.hpp"
#include "VertexArray.hpp"

#include <stdint.h>

namespace RIS
{
    class Mesh
    {
    public:
        Mesh(const Buffer &vertexBuffer, const Buffer &indexBuffer, int numIndices, bool isStatic);

        Mesh(const Mesh &) = delete;
        Mesh(Mesh &&) = default;
        Mesh &operator=(const Mesh &) = delete;
        Mesh &operator=(Mesh &&) = default;

        void Bind(VertexArray &vao);

        const Buffer& VertexBuffer() const;
        const Buffer& IndexBuffer() const;
        int NumIndices() const;

    private:
        const Buffer &vertexBuffer;
        const Buffer &indexBuffer;
        const int numIndices;
        const bool isStatic;
    };
}
