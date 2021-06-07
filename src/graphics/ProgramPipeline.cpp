#include "graphics/ProgramPipeline.hpp"

#include <glad2/gl.h>

namespace RIS::Graphics
{
    ProgramPipeline::ProgramPipeline()
        : GLObject(0)
    {
        glCreateProgramPipelines(1, &id);
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

    void ProgramPipeline::SetShader(const Shader &shader)
    {
        glUseProgramStages(id, shader.GetType(), shader.GetId());
    }

    void ProgramPipeline::Use()
    {
        glBindProgramPipeline(id);
    }
}
