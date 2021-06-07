#include "graphics/VertexArray.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RIS::Graphics
{
    VertexArray::VertexArray()
        : GLObject(0)
    {
        glCreateVertexArrays(1, &id);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &id);
    }

    VertexArray::VertexArray(VertexArray &&other)
    {
        std::swap(id, other.id);
    }

    VertexArray& VertexArray::operator=(VertexArray &&other)
    {
        std::swap(id, other.id);
        return *this;
    }

    void VertexArray::SetAttribFormat(int attrib, int numComponents, GLenum type, std::size_t offset, bool normalized, int bindingPoint)
    {
        glEnableVertexArrayAttrib(id, attrib);
        if(type == GL_FLOAT)
            glVertexArrayAttribFormat(id, attrib, numComponents, type, normalized, static_cast<GLuint>(offset));
        else if(type == GL_DOUBLE)
            glVertexArrayAttribLFormat(id, attrib, numComponents, type, static_cast<GLuint>(offset));
        else
            glVertexArrayAttribIFormat(id, attrib, numComponents, type, static_cast<GLuint>(offset));
        glVertexArrayAttribBinding(id, attrib, bindingPoint);
    }

    void VertexArray::Bind()
    {
        glBindVertexArray(id);
    }

    void VertexArray::SetIndexBuffer(const Buffer &buffer)
    {
        glVertexArrayElementBuffer(id, buffer.GetId());
    }
}
