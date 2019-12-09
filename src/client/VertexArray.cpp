#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    VertexArray::VertexArray()
        : vertexArrayId(0)
    {
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &vertexArrayId);
    }

    VertexArray::VertexArray(VertexArray &&other)
    {
        vertexArrayId = other.vertexArrayId;
        other.vertexArrayId = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray &&other)
    {
        vertexArrayId = other.vertexArrayId;
        other.vertexArrayId = 0;
        return *this;
    }

    void VertexArray::Create()
    {
        glCreateVertexArrays(1, &vertexArrayId);
    }

    void VertexArray::SetVertexBuffer(const Buffer &buffer, int bindingPoint)
    {
        glVertexArrayVertexBuffer(vertexArrayId, bindingPoint, buffer.GetId(), 0, buffer.GetElementSize());
    }

    void VertexArray::SetIndexBuffer(const Buffer &buffer)
    {
        glVertexArrayElementBuffer(vertexArrayId, buffer.GetId());
    }

    void VertexArray::SetAttribFormat(int attrib, int numComponents, gl::GLenum type, std::size_t offset, int bindingPoint)
    {
        glEnableVertexArrayAttrib(vertexArrayId, attrib);
        glVertexArrayAttribFormat(vertexArrayId, attrib, numComponents, type, GL_FALSE, offset);
        glVertexArrayAttribBinding(vertexArrayId, attrib, bindingPoint);
    }

    void VertexArray::Bind()
    {
        glBindVertexArray(vertexArrayId);
    }

    GLuint VertexArray::GetId() const
    {
        return vertexArrayId;
    }
}
