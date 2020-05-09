#pragma once

#include "graphics/GLObject.hpp"

#include <stdexcept>

#include <glad/glad.h>

#include <unordered_map>
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
            template<typename T>
            Buffer(const T &data, GLenum usage);
            template<typename T>
            Buffer(const std::vector<T> &data, GLenum usage);

            Buffer();
            ~Buffer();

            Buffer(const Buffer &) = delete;
            Buffer& operator=(const Buffer &) = delete;

            Buffer(Buffer &&other);
            Buffer& operator=(Buffer &&other);

            void UpdateData(const void *data, size_t size, size_t offset = 0);

            template<typename T>
            void UpdateData(const T &data, size_t offset = 0);
            template<typename T>
            void UpdateData(const std::vector<T> &data, size_t offset = 0);

            void Bind(GLenum target, int bindBase);

            void* Map(GLenum access, size_t size = 0, size_t offset = 0);
            void UnMap();
            void Flush(size_t size = 0, size_t offset = 0);

            size_t GetSize() const;

        private:
            size_t maxSize;

        };

        template<typename T>
        void Buffer::UpdateData(const T &data, size_t offset)
        {
            UpdateData(&data, sizeof T, offset);
        }

        template<typename T>
        void Buffer::UpdateData(const std::vector<T> &data, size_t offset)
        {
            UpdateData(data.data(), data.size() * sizeof T, offset);
        }

        template<typename T>
        Buffer::Buffer(const T &data, GLenum usage)
            : Buffer(&data, sizeof T, usage)
        {}

        template<typename T>
        Buffer::Buffer(const std::vector<T> &data, GLenum usage)
            : Buffer(data.data(), data.size() * sizeof T, usage)
        {}
    }
}
