#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    Buffer::Buffer()
        : bufferId(0), elementSize(0)
    {
    }

    Buffer::~Buffer()
    {
        glDeleteBuffers(1, &bufferId);
    }

    Buffer::Buffer(Buffer &&other)
    {
        bufferId = other.bufferId;
        other.bufferId = 0;
    }

    Buffer& Buffer::operator=(Buffer &&other)
    {
        bufferId = other.bufferId;
        other.bufferId = 0;
        return *this;
    }

    void Buffer::Create()
    {
        glCreateBuffers(1, &bufferId);
    }

    void Buffer::Reserve(gl::GLenum usage, std::size_t size)
    {
        glNamedBufferData(bufferId, size, nullptr, usage);
    }

    void Buffer::Bind(int bindBase)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindBase, bufferId);
    }

    std::size_t Buffer::GetElementSize() const
    {
        return elementSize;
    }

    GLuint Buffer::GetId() const
    {
        return bufferId;
    }
}
