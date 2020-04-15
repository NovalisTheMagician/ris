#include "GLRenderer.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#include <tiny_gltf.h>

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <experimental/unordered_map>

#include "VertexTypes.hpp"

#include "common/Logger.hpp"

#include "common/ILoader.hpp"
#include "common/LuaScriptEngine.hpp"

using namespace gl46core;
using namespace std::literals;

namespace RIS
{
    const ResourceId GL3DRenderer::MISSING_MODEL_ID = 0;

    GL3DRenderer::GL3DRenderer(GLRenderer &renderer)
        : renderer(renderer)
    {

    }

    GL3DRenderer::~GL3DRenderer()
    {
        models.clear();
    }

    void GL3DRenderer::Setup()
    {
        modelVAO = VertexArray::Create();
        modelVAO.SetAttribFormat(0, 3, GL_FLOAT, offsetof(VertexType::ModelVertex, position));
        modelVAO.SetAttribFormat(1, 3, GL_FLOAT, offsetof(VertexType::ModelVertex, normal));
        modelVAO.SetAttribFormat(2, 2, GL_FLOAT, offsetof(VertexType::ModelVertex, texCoords));
        modelVAO.SetAttribFormat(3, 4, GL_INT, offsetof(VertexType::ModelVertex, joints));
        modelVAO.SetAttribFormat(4, 4, GL_FLOAT, offsetof(VertexType::ModelVertex, weights));

        Buffer emptyVBO = Buffer<VertexType::ModelVertex>::CreateImmutable({glm::vec3(), glm::vec3(), glm::vec2()}, GL_DYNAMIC_STORAGE_BIT);
        Buffer emptyIBO = Buffer<uint16_t>::CreateImmutable(static_cast<uint16_t>(0), GL_DYNAMIC_STORAGE_BIT);
        vertexBuffers.insert(vertexBuffers.begin(), std::move(emptyVBO));
        indexBuffers.insert(indexBuffers.begin(), std::move(emptyIBO));

        models.insert({MISSING_MODEL_ID, Mesh(vertexBuffers.front(), indexBuffers.front(), 1, true)});

        perFrameBuffer = Buffer<PerFrameMatrices>::CreateImmutable(perFrameData, GL_DYNAMIC_STORAGE_BIT);
        perObjectBuffer = Buffer<PerObjectMatrices>::CreateImmutable(perObjectData, GL_DYNAMIC_STORAGE_BIT);

        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);

        Config &config = renderer.config;
        float aniso = config.GetValue("r_anisotropic", maxAniso);
        if(aniso > maxAniso)
        {
            aniso = maxAniso;
            config.SetValue("r_anisotropic", aniso);
        }

        defaultSampler = Sampler::Create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, aniso);
    }

    void GL3DRenderer::LoadShaders()
    {
        auto createShader = [this](auto &future, gl::GLenum shaderType)
        {
            auto [data, size] = future.get();
            return Shader::Create(data.get(), size, shaderType, renderer.useAmdFix);
        };

        ILoader &loader = renderer.systems.GetLoader();

        AssetType shaderLoadType = renderer.useAmdFix ? AssetType::SHADERSRC : AssetType::SHADER;

        auto shaderMStaticVertFut = loader.LoadAsset(shaderLoadType, "mStaticVert");
        auto shaderMUnlitFut = loader.LoadAsset(shaderLoadType, "mUnlit");

        staticModelShader = createShader(shaderMStaticVertFut, GL_VERTEX_SHADER);
        modelUnlitShader = createShader(shaderMUnlitFut, GL_FRAGMENT_SHADER);
    }

    ResourceId GL3DRenderer::LoadMesh(const std::string &name)
    {
        if(loadedModels.count(name) == 1)
            return loadedModels.at(name);

        Logger &logger = Logger::Instance();
        ILoader &loader = renderer.systems.GetLoader();
        try
        {
            auto [data, size] = loader.LoadAsset(AssetType::MODEL, name).get();

            tinygltf::TinyGLTF gltfLoader;
            tinygltf::Model model;
            std::string err, warn;

            bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<unsigned char*>(data.get()), size);
            if(!warn.empty()) logger.Warning(warn);
            if(!err.empty()) logger.Error(err);
            if(result)
            {
                auto checkAttribs = [](const std::map<std::string, int> &attribs)
                {
                    int hasAll = 0;
                    std::for_each(attribs.begin(), attribs.end(), [&hasAll](const std::pair<std::string, int> attrib)
                    {
                        if(attrib.first.compare("POSITION") == 0) hasAll++;
                        else if(attrib.first.compare("NORMAL") == 0) hasAll++;
                        else if(attrib.first.compare("TEXCOORD_0") == 0) hasAll++;
                    });
                    return hasAll == 3;
                };

                auto transformBytesToShorts = [](auto begin, auto end)
                {
                    size_t size = end - begin;
                    std::vector<unsigned short> shorts(size / 2);
                    for(size_t i = 0; i < size / 2; ++i)
                    {
                        size_t idx = i * 2;
                        auto msb = *(begin + (idx+1));
                        auto lsb = *(begin + idx);
                        shorts[i] = msb << 8 | lsb;
                    }
                    return shorts;
                };

                auto transformBytesToFloats = [](auto begin, auto end)
                {
                    size_t size = end - begin;
                    std::vector<float> floats(size / 4);
                    for(size_t i = 0; i < size / 4; ++i)
                    {
                        size_t idx = i * 4;
                        auto d = *(begin + (idx+3));
                        auto c = *(begin + (idx+2));
                        auto b = *(begin + (idx+1));
                        auto a = *(begin + idx);
                        unsigned char arr[] = { a, b, c, d };
                        float result;
                        std::copy(  reinterpret_cast<const char*>(&arr[0]),
                                    reinterpret_cast<const char*>(&arr[4]),
                                    reinterpret_cast<char*>(&result));
                        floats[i] = result;
                    }
                    return floats;
                };

                auto transformFloatsToVec3 = [](const std::vector<float> &floats)
                {
                    std::vector<glm::vec3> vectors(floats.size() / 3);
                    for(size_t i = 0; i < floats.size() / 3; ++i)
                    {
                        size_t idx = i * 3;
                        auto x = floats.at(idx);
                        auto y = floats.at(idx+1);
                        auto z = floats.at(idx+2);
                        vectors[i] = glm::vec3(x, y, z);
                    }
                    return vectors;
                };

                auto transformFloatsToVec2 = [](const std::vector<float> &floats)
                {
                    std::vector<glm::vec2> vectors(floats.size() / 2);
                    for(size_t i = 0; i < floats.size() / 2; ++i)
                    {
                        size_t idx = i * 2;
                        auto x = floats.at(idx);
                        auto y = floats.at(idx+1);
                        vectors[i] = glm::vec2(x, y);
                    }
                    return vectors;
                };

                auto zipToModelFormat = [](const std::vector<glm::vec3> &positions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &texcoords)
                {
                    std::vector<VertexType::ModelVertex> vertices(positions.size());
                    for(size_t i = 0; i < positions.size(); ++i)
                    {
                        vertices[i] = { positions.at(i), normals.at(i), texcoords.at(i) };
                    }
                    return vertices;
                };

                ResourceId id = highestUnusedModelId++;
                const tinygltf::Mesh &mesh = model.meshes.at(0);
                const tinygltf::Primitive &primitive = mesh.primitives.at(0);

                if(!checkAttribs(primitive.attributes))
                    throw std::exception("model in wrong format");

                std::vector<uint16_t> indices;
                std::vector<glm::vec3> positions;
                std::vector<glm::vec3> normals;
                std::vector<glm::vec2> texcoords;

                size_t index = 0;
                for(const tinygltf::BufferView &bufferView : model.bufferViews)
                {
                    const tinygltf::Buffer buffer = model.buffers.at(bufferView.buffer);
                    auto begin = buffer.data.begin() + bufferView.byteOffset;
                    auto end = buffer.data.begin() + bufferView.byteOffset + bufferView.byteLength;

                    if(index == 0) // POSITION VEC3
                    {
                        positions = transformFloatsToVec3(transformBytesToFloats(begin, end));
                    }
                    else if(index == 1) // NORMAL VEC3
                    {
                        normals = transformFloatsToVec3(transformBytesToFloats(begin, end));
                    }
                    else if(index == 3) // TEXCOORD VEC2
                    {
                        texcoords = transformFloatsToVec2(transformBytesToFloats(begin, end));
                    }
                    else if(index == 4) // INDEX USHORT
                    {
                        indices.resize(bufferView.byteLength);
                        indices = transformBytesToShorts(begin, end);
                    }
                    index++;
                }

                std::vector<VertexType::ModelVertex> vertices = zipToModelFormat(positions, normals, texcoords);

                Buffer vertexBuffer = Buffer<VertexType::ModelVertex>::CreateImmutable(vertices, GL_DYNAMIC_STORAGE_BIT);
                Buffer indexBuffer = Buffer<uint16_t>::CreateImmutable(indices, GL_DYNAMIC_STORAGE_BIT);

                vertexBuffers.insert(vertexBuffers.end(), std::move(vertexBuffer));
                indexBuffers.insert(indexBuffers.end(), std::move(indexBuffer));
                
                int numIndices = model.accessors.at(primitive.indices).count;

                models.insert({id, Mesh(vertexBuffers.back(), indexBuffers.back(), numIndices, true)});
                loadedModels.insert({name, id});

                return id;
            }
        }
        catch(const std::exception &e)
        {
            Logger::Instance().Error("Failed to load model (" + name + "): "s + e.what());
        }
        return MISSING_MODEL_ID;
    }

    void GL3DRenderer::DestroyMesh(ResourceId modelId)
    {
        using std::experimental::erase_if;

        if(modelId >= 0 && models.count(modelId) > 0)
        {
            models.erase(modelId);
            erase_if(loadedModels, [modelId](const std::pair<std::string, ResourceId> &elem){ return elem.second == modelId; });
        }
    }

    void GL3DRenderer::Begin(const glm::mat4 &viewProjection)
    {
        perFrameData.viewProjection = viewProjection;
        perFrameBuffer.UpdateData(perFrameData);

        defaultSampler.Bind(0);
    }

    void GL3DRenderer::End()
    {

    }

    void GL3DRenderer::Draw(ResourceId modelId, const glm::mat4 &world)
    {
        perObjectData.world = world;
        perObjectBuffer.UpdateData(perObjectData);

        perFrameBuffer.Bind(GL_UNIFORM_BUFFER, 0);
        perObjectBuffer.Bind(GL_UNIFORM_BUFFER, 1);

        Mesh &model = models.at(modelId);
        model.Bind(modelVAO);
        modelVAO.Bind();

        renderer.pipeline.SetShader(staticModelShader);
        renderer.pipeline.SetShader(modelUnlitShader);
        renderer.pipeline.Use();

        glDrawElements(GL_TRIANGLES, model.NumIndices(), GL_UNSIGNED_SHORT, nullptr);
    }
}
