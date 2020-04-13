#pragma once

//#include "GLRenderer.hpp"
#include "Buffer.hpp"
#include "VertexTypes.hpp"
#include "VertexArray.hpp"

#include <stdint.h>

namespace RIS
{
    class Model
    {
    public:
        Model(const Buffer<VertexType::ModelVertex> &vertexBuffer, const Buffer<uint16_t> &indexBuffer, int numIndices, bool isStatic);

        Model(const Model &) = delete;
        Model(Model &&) = default;
        Model &operator=(const Model &) = delete;
        Model &operator=(Model &&) = default;

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
