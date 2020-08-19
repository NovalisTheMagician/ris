#pragma once

#include "graphics/GLObject.hpp"

#include <stdexcept>

#include <glad/glad.h>

#include <vector>

namespace RIS
{
    namespace Graphics
    {
        class Buffer : public GLObject
        {
        public:
            Buffer(const void *data, size_t size, GLenum usage);
            Buffer(size_t size, GLenum usage);
            template<typename T, std::size_t Size = sizeof T>
            Buffer(const T &data, GLenum usage);
            template<typename T, std::size_t Size = sizeof T>
            Buffer(const std::vector<T> &data, GLenum usage);

            Buffer();
            ~Buffer();

            Buffer(const Buffer &) = delete;
            Buffer& operator=(const Buffer &) = delete;

            Buffer(Buffer &&other);
            Buffer& operator=(Buffer &&other);

            void UpdateData(const void *data, size_t size, size_t offset = 0);

            template<typename T, std::size_t Size = sizeof T>
            void UpdateData(const T &data, size_t offset = 0);
            template<typename T, std::size_t Size = sizeof T>
            void UpdateData(const std::vector<T> &data, size_t offset = 0);

            void Bind(GLenum target, int bindBase);

            void* Map(GLenum access, size_t size = 0, size_t offset = 0);
            void UnMap();
            void Flush(size_t size = 0, size_t offset = 0);

            size_t GetSize() const;

        private:
            size_t maxSize;

        };

        template<typename T, std::size_t Size>
        void Buffer::UpdateData(const T &data, size_t offset)
        {
            UpdateData(&data, Size, offset);
        }

        template<typename T, std::size_t Size>
        void Buffer::UpdateData(const std::vector<T> &data, size_t offset)
        {
            UpdateData(data.data(), data.size() * Size, offset);
        }

        template<typename T, std::size_t Size>
        Buffer::Buffer(const T &data, GLenum usage)
            : Buffer(&data, Size, usage)
        {}

        template<typename T, std::size_t Size>
        Buffer::Buffer(const std::vector<T> &data, GLenum usage)
            : Buffer(data.data(), data.size() * Size, usage)
        {}
    }
}
