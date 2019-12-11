#include "VertexArray.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    VertexArray::VertexArray()
        : GLObject(0)
    {
    }

    VertexArray::VertexArray(GLuint id)
        : GLObject(id)
    {
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &id);
    }

    VertexArray::VertexArray(VertexArray &&other)
    {
        id = other.id;
        other.id = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray &&other)
    {
        id = other.id;
        other.id = 0;
        return *this;
    }

    VertexArray VertexArray::Create()
    {
        GLuint id;
        glCreateVertexArrays(1, &id);
        return VertexArray(id);
    }

    void VertexArray::SetAttribFormat(int attrib, int numComponents, gl::GLenum type, std::size_t offset, int bindingPoint)
    {
        glEnableVertexArrayAttrib(id, attrib);
        glVertexArrayAttribFormat(id, attrib, numComponents, type, GL_FALSE, offset);
        glVertexArrayAttribBinding(id, attrib, bindingPoint);
    }

    void VertexArray::Bind()
    {
        glBindVertexArray(id);
    }
}
