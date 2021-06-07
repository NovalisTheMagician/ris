#include "graphics/Buffer.hpp"

#include <glad2/gl.h>

namespace RIS::Graphics
{
    Buffer::Buffer(const void *data, size_t size, GLenum usage)
        : maxSize(size)
    {
        glCreateBuffers(1, &id);
        glNamedBufferStorage(id, size, data, usage);
    }

    Buffer::Buffer(size_t size, GLenum usage)
        : maxSize(size)
    {
        glCreateBuffers(1, &id);
        glNamedBufferStorage(id, size, nullptr, usage);
    }

    Buffer::Buffer() 
        : GLObject(0), maxSize(0)
    {}

    Buffer::~Buffer() 
    { 
        glDeleteBuffers(1, &id);
    }

    Buffer::Buffer(Buffer &&other)
        : maxSize(other.maxSize)
    {
        std::swap(id, other.id);
    }

    Buffer& Buffer::operator=(Buffer &&other)
    {
        std::swap(id, other.id);
        std::swap(maxSize, other.maxSize);
        return *this;
    }

    void Buffer::UpdateData(const void *data, size_t size, size_t offset)
    {
        glNamedBufferSubData(id, offset, size, data);
    }

    void Buffer::Bind(GLenum target, int bindBase) const
    { 
        glBindBufferBase(target, bindBase, id); 
    }

    size_t Buffer::GetSize() const
    {
        return maxSize;
    }

    void* Buffer::Map(GLenum access, size_t size, size_t offset)
    {
        return glMapNamedBufferRange(id, offset, size == 0 ? maxSize : size, access);
    }

    void Buffer::UnMap()
    {
        glUnmapNamedBuffer(id);
    }

    void Buffer::Flush(size_t size, size_t offset)
    {
        glFlushMappedNamedBufferRange(id, offset, size == 0 ? maxSize : size);
    }
}
