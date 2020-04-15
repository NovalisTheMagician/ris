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
        Mesh(const Buffer<VertexType::ModelVertex> &vertexBuffer, const Buffer<uint16_t> &indexBuffer, int numIndices, bool isStatic);

        Mesh(const Mesh &) = delete;
        Mesh(Mesh &&) = default;
        Mesh &operator=(const Mesh &) = delete;
        Mesh &operator=(Mesh &&) = default;

        void Bind(VertexArray &vao);

        const Buffer<VertexType::ModelVertex>& VertexBuffer() const;
        const Buffer<uint16_t>& IndexBuffer() const;
        int NumIndices() const;

    private:
        const Buffer<VertexType::ModelVertex> &vertexBuffer;
        const Buffer<uint16_t> &indexBuffer;
        const int numIndices;
        const bool isStatic;
    };
}
