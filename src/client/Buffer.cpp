#include "Buffer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

using namespace gl46core;

namespace RIS
{
    Buffer::Buffer(const void *data, size_t size, gl::BufferStorageMask usage)
        : maxSize(size)
    {
        glCreateBuffers(1, &id);
        glNamedBufferStorage(id, size, data, usage);
    }

    Buffer::Buffer(size_t size, gl::BufferStorageMask usage)
        : maxSize(size)
    {
        glCreateBuffers(1, &id);
        glNamedBufferStorage(id, size, nullptr, usage);
    }

    Buffer::Buffer() 
        : GLObject(0), maxSize(0)
    {

    }

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
        return *this;
    }

    void Buffer::UpdateData(const void *data, size_t size, size_t offset)
    {
        glNamedBufferSubData(id, offset, size, data);
    }

    void Buffer::Bind(gl::GLenum target, int bindBase) 
    { 
        glBindBufferBase(target, bindBase, id); 
    }

    Buffer Buffer::Create(const void *data, size_t size, gl::BufferStorageMask usage) 
    { 
        return Buffer(data, size, usage); 
    }

    Buffer Buffer::Create(size_t size, gl::BufferStorageMask usage) 
    { 
        return Buffer(size, usage); 
    }

    size_t Buffer::GetSize() const
    {
        return maxSize;
    }
}
