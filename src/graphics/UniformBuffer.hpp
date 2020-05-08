#pragma once

#include "graphics/Buffer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#include <vector>

namespace RIS
{
    namespace Graphics
    {
        template<typename T>
        class UniformBuffer
        {
        public:
            UniformBuffer()
            {
                buffer = Buffer::Create(data, gl::GL_MAP_PERSISTENT_BIT | gl::GL_MAP_COHERENT_BIT | gl::GL_MAP_WRITE_BIT);
                bufferPtr = buffer.Map(gl::GL_MAP_WRITE_BIT | gl::GL_MAP_PERSISTENT_BIT | gl::GL_MAP_COHERENT_BIT | gl::GL_MAP_INVALIDATE_BUFFER_BIT);
            };

            UniformBuffer(const T &data)
                : data(data)
            {
                buffer = Buffer::Create(data, gl::GL_MAP_PERSISTENT_BIT | gl::GL_MAP_COHERENT_BIT | gl::GL_MAP_WRITE_BIT);
                bufferPtr = buffer.Map(gl::GL_MAP_WRITE_BIT | gl::GL_MAP_PERSISTENT_BIT | gl::GL_MAP_COHERENT_BIT | gl::GL_MAP_INVALIDATE_BUFFER_BIT);
            }

            ~UniformBuffer()
            {
                buffer.UnMap();
            }

            UniformBuffer(const UniformBuffer&) = delete;
            UniformBuffer& operator=(const UniformBuffer&) = delete;
            UniformBuffer(UniformBuffer&&) = default;
            UniformBuffer& operator=(UniformBuffer&&) = default;

            void Bind(int bindBase)
            {
                buffer.Bind(gl::GL_UNIFORM_BUFFER, bindBase);
            }

            T& Get()
            {
                return data;
            }

            void Update(const T &newData)
            {
                data = newData;
                Update();
            }

            void Update()
            {
                std::memcpy(bufferPtr, &data, sizeof T);
            }

        private:
            T data;
            Buffer buffer;
            void *bufferPtr;
        };

        template<typename T>
        class UniformBuffer<std::vector<T>>
        {
        public:
            UniformBuffer() = delete;
            UniformBuffer(const std::vector<T> &data)
                : data(data)
            {
                buffer = Buffer::Create(data, gl::GL_MAP_PERSISTENT_BIT | gl::GL_MAP_COHERENT_BIT | gl::GL_MAP_WRITE_BIT);
                bufferPtr = buffer.Map(gl::GL_MAP_WRITE_BIT | gl::GL_MAP_PERSISTENT_BIT | gl::GL_MAP_COHERENT_BIT | gl::GL_MAP_INVALIDATE_BUFFER_BIT);
            }

            ~UniformBuffer()
            {
                buffer.UnMap();
            }

            UniformBuffer(const UniformBuffer&) = delete;
            UniformBuffer& operator=(const UniformBuffer&) = delete;
            UniformBuffer(UniformBuffer&&) = default;
            UniformBuffer& operator=(UniformBuffer&&) = default;

            void Bind(int bindBase)
            {
                buffer.Bind(gl::GL_UNIFORM_BUFFER, bindBase);
            }

            std::vector<T>& Get()
            {
                return data;
            }

            void Update(const std::vector<T> &newData)
            {
                data = newData;
                Update();
            }

            void Update()
            {
                std::memcpy(bufferPtr, data.data(), data.size() * sizeof T);
            }

        private:
            std::vector<T> data;
            Buffer buffer;
            void *bufferPtr;
        };
    }
}
