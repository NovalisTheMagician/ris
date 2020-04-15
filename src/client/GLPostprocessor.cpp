#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/Logger.hpp"

#include "common/ILoader.hpp"
#include "common/LuaScriptEngine.hpp"

using namespace gl46core;
using namespace std::literals;

namespace RIS
{
    GLPostProcessor::GLPostProcessor(GLRenderer &renderer)
        : renderer(renderer)
    {

    }

    GLPostProcessor::~GLPostProcessor()
    {

    }

    void GLPostProcessor::Setup()
    {
        std::vector<VertexType::UIVertex> vertices(6);
        vertices[0] = {{1, 1}, {1, 1}};
        vertices[1] = {{-1, 1}, {0, 1}};
        vertices[2] = {{-1, -1}, {0, 0}};
        vertices[3] = {{1, -1}, {1, 0}};
        vertices[4] = {{1, 1}, {1, 1}};
        vertices[5] = {{-1, -1}, {0, 0}};
        fullscreenQuad = Buffer<VertexType::UIVertex>::CreateImmutable(vertices, GL_DYNAMIC_STORAGE_BIT);

        postprocessVAO = VertexArray::Create();
        postprocessVAO.SetAttribFormat(0, 2, GL_FLOAT, offsetof(VertexType::UIVertex, position));
        postprocessVAO.SetAttribFormat(1, 2, GL_FLOAT, offsetof(VertexType::UIVertex, texCoords));
        postprocessVAO.SetVertexBuffer(fullscreenQuad, 0);

        sampler = Sampler::Create(GL_LINEAR, GL_LINEAR, 0.0f);
    }
    
    void GLPostProcessor::LoadShaders()
    {
        auto createShader = [this](auto &future, gl::GLenum shaderType)
        {
            auto [data, size] = future.get();
            return Shader::Create(data.get(), size, shaderType, renderer.useAmdFix);
        };

        ILoader &loader = renderer.systems.GetLoader();

        AssetType shaderLoadType = renderer.useAmdFix ? AssetType::SHADERSRC : AssetType::SHADER;

        auto shaderPpvFut = loader.LoadAsset(shaderLoadType, "ppVert");
        auto shaderPpcFut = loader.LoadAsset(shaderLoadType, "ppCopy");

        ppVertex = createShader(shaderPpvFut, GL_VERTEX_SHADER);
        ppCopy = createShader(shaderPpcFut, GL_FRAGMENT_SHADER);
    }

    void GLPostProcessor::SetFramebufferTexture(ResourceId framebufId, uint32_t place)
    {
        auto &fb = renderer.framebuffers.at(framebufId);
        fb.GetColorTexture().Bind(place);
        sampler.Bind(place);
    }

    void GLPostProcessor::Begin()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void GLPostProcessor::End()
    {
        glDisable(GL_BLEND);
    }
    
    void GLPostProcessor::Draw(int pp)
    {
        if(pp == 0) // copy
        {
            postprocessVAO.Bind();

            renderer.pipeline.SetShader(ppVertex);
            renderer.pipeline.SetShader(ppCopy);
            renderer.pipeline.Use();

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}
