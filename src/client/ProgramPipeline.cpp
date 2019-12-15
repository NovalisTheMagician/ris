#include "ProgramPipeline.hpp"

#include <glbinding/gl46core/gl.h>

using namespace gl46core;

namespace RIS
{
    ProgramPipeline::ProgramPipeline()
        : GLObject(0)
    {
    }

    ProgramPipeline::ProgramPipeline(GLuint id)
        : GLObject(id)
    {
    }

    ProgramPipeline::~ProgramPipeline()
    {
        glDeleteProgramPipelines(1, &id);
    }

    ProgramPipeline::ProgramPipeline(ProgramPipeline &&other)
    {
        std::swap(id, other.id);
    }

    ProgramPipeline& ProgramPipeline::operator=(ProgramPipeline &&other)
    {
        std::swap(id, other.id);
        return *this;
    }

    ProgramPipeline ProgramPipeline::Create()
    {
        GLuint id;
        glCreateProgramPipelines(1, &id);
        return ProgramPipeline(id);
    }

    void ProgramPipeline::SetShader(const Shader &shader)
    {
        glUseProgramStages(id, shader.GetType(), shader.GetId());
    }

    void ProgramPipeline::Use()
    {
        glBindProgramPipeline(id);
    }
}
