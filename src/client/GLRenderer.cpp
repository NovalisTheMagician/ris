#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#include <tiny_gltf.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gli/gli.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <experimental/unordered_map>

#include "common/Logger.hpp"

#include "common/ILoader.hpp"
#include "common/LuaScriptEngine.hpp"

#include "VertexTypes.hpp"

using namespace gl46core;
using namespace std::literals;

// Windows "hack" to force some laptops to use the highperformance GPU
// some intel integrated gpus don't support opengl 4.6
// I might, in the future, reduce the required opengl version
#ifdef _WIN32
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace RIS
{
    const ResourceId GLRenderer::DEFAULT_FRAMBUFFER_ID = 0;
    const ResourceId GLRenderer::DEFAULT_TEXTURE_ID = 1;
    const ResourceId GLRenderer::MISSING_TEXTURE_ID = 0;
    const ResourceId GLRenderer::MISSING_MODEL_ID = 0;

    GLRenderer::GLRenderer(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), renderer2d(*this)
    {
        auto &log = Logger::Instance();

        glbinding::initialize(glfwGetProcAddress);

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        auto dbgCallback = [](GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const void *userParam) -> void
            {
                auto const src_str = [source]() {
                    switch (source)
                    {
                    case GL_DEBUG_SOURCE_API: return "API";
                    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
                    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
                    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
                    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
                    case GL_DEBUG_SOURCE_OTHER: return "OTHER";
                    default: return "";
                    }
                }();

                auto const type_str = [type]() {
                    switch (type)
                    {
                    case GL_DEBUG_TYPE_ERROR: return "ERROR";
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
                    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
                    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
                    case GL_DEBUG_TYPE_MARKER: return "MARKER";
                    case GL_DEBUG_TYPE_OTHER: return "OTHER";
                    default: return "";
                    }
                }();

                auto const severity_str = [severity]() {
                    switch (severity) {
                    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
                    case GL_DEBUG_SEVERITY_LOW: return "LOW";
                    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
                    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
                    default: return "";
                    }
                }();

                std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << std::endl;
            };

        glDebugMessageCallback(static_cast<GLDEBUGPROC>(dbgCallback), nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

        //glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        if(vendor.find("ATI") != std::string::npos || vendor.find("ati") != std::string::npos)
            useAmdFix = true;

        if(static_cast<const Config&>(config).GetValue("g_ignoreamdfix", false))
            useAmdFix = false;

        log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);

        renderer2d.Setup();

        int width = config.GetValue("r_width", 800);
        int height = config.GetValue("r_height", 600);
        Resize(width, height);

        framebuffers.emplace(DEFAULT_FRAMBUFFER_ID, 0);

        textures.insert({DEFAULT_TEXTURE_ID, Texture::Create(glm::vec4(1, 1, 1, 1))});
        textures.insert({MISSING_TEXTURE_ID, Texture::Create(glm::vec4(1, 0, 1, 1))});

        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);

        float aniso = config.GetValue("r_anisotropic", maxAniso);
        if(aniso > maxAniso)
        {
            aniso = maxAniso;
            config.SetValue("r_anisotropic", aniso);
        }

        defaultSampler = Sampler::Create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, aniso);
        uiSampler = Sampler::Create(GL_LINEAR, GL_LINEAR, 1.0f);

        pipeline = ProgramPipeline::Create();

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

        models.insert({MISSING_MODEL_ID, Model(vertexBuffers.front(), indexBuffers.front(), 1, true)});

        perFrameBuffer = Buffer<PerFrameMatrices>::CreateImmutable(perFrameData, GL_DYNAMIC_STORAGE_BIT);
        perObjectBuffer = Buffer<PerObjectMatrices>::CreateImmutable(perObjectData, GL_DYNAMIC_STORAGE_BIT);

        GLenum enc;
        glGetNamedFramebufferAttachmentParameteriv(0, GL_FRONT_LEFT, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &enc);
        if(enc != GL_SRGB)
        {
            Logger::Instance().Warning("Window is not sRGB capable!");
        }
    }

    GLRenderer::~GLRenderer()
    {
        textures.clear();
    }

    void GLRenderer::PostInit()
    {
        auto createShader = [this](auto &future, gl::GLenum shaderType)
        {
            auto [data, size] = future.get();
            return Shader::Create(data.get(), size, shaderType, useAmdFix);
        };

        ILoader &loader = systems.GetLoader();

        AssetType shaderLoadType = useAmdFix ? AssetType::SHADERSRC : AssetType::SHADER;

        auto shaderUivFut = loader.LoadAsset(shaderLoadType, "uiVert");
        auto shaderUifFut = loader.LoadAsset(shaderLoadType, "uiFrag");
        auto shaderUitFut = loader.LoadAsset(shaderLoadType, "uiText");
        auto shaderPpvFut = loader.LoadAsset(shaderLoadType, "ppVert");
        auto shaderPpcFut = loader.LoadAsset(shaderLoadType, "ppCopy");

        auto shaderMStaticVertFut = loader.LoadAsset(shaderLoadType, "mStaticVert");
        auto shaderMUnlitFut = loader.LoadAsset(shaderLoadType, "mUnlit");

        uiVertex = createShader(shaderUivFut, GL_VERTEX_SHADER);
        uiFragment = createShader(shaderUifFut, GL_FRAGMENT_SHADER);
        uiText = createShader(shaderUitFut, GL_FRAGMENT_SHADER);
        ppVertex = createShader(shaderPpvFut, GL_VERTEX_SHADER);
        ppCopy = createShader(shaderPpcFut, GL_FRAGMENT_SHADER);

        staticModelShader = createShader(shaderMStaticVertFut, GL_VERTEX_SHADER);
        modelUnlitShader = createShader(shaderMUnlitFut, GL_FRAGMENT_SHADER);

        LuaScriptEngine &scriptEngine = dynamic_cast<LuaScriptEngine&>(systems.GetScriptEngine());

        scriptEngine.Register([this](const char *name){ return LoadTexture(name); }, "renderer", "loadTexture");
        scriptEngine.Register([this](const char *name){ return renderer2d.LoadFont(name); }, "renderer", "loadFont");
    }

    ResourceId GLRenderer::LoadTexture(const std::string &name, bool flip)
    {
        if(loadedTextures.count(name) == 1)
            return loadedTextures.at(name);

        ILoader &loader = systems.GetLoader();
        try
        {
            auto [data, size] = loader.LoadAsset(AssetType::TEXTURE, name).get();

            ResourceId id = highestUnusedTexId++;
            textures[id] = Texture::Create(data.get(), size, flip);
            loadedTextures.insert({name, id});
            return id;
        }
        catch(const std::exception& e)
        {
            Logger::Instance().Error("Failed to load texture (" + name + "): "s + e.what());
        }
        return MISSING_TEXTURE_ID;
    }

    void GLRenderer::DestroyTexture(ResourceId texId)
    {
        using std::experimental::erase_if;

        if(texId >= 0 && textures.count(texId) > 0)
        {
            textures.erase(texId);
            erase_if(loadedTextures, [texId](const std::pair<std::string, ResourceId> &elem){ return elem.second == texId; });
        }
    }

    ResourceId GLRenderer::LoadModel(const std::string &name)
    {
        if(loadedModels.count(name) == 1)
            return loadedModels.at(name);

        Logger &logger = Logger::Instance();
        ILoader &loader = systems.GetLoader();
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

                models.insert({id, Model(vertexBuffers.back(), indexBuffers.back(), numIndices, true)});
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

    void GLRenderer::DestroyModel(ResourceId modId)
    {
        using std::experimental::erase_if;

        if(modId >= 0 && models.count(modId) > 0)
        {
            models.erase(modId);
            erase_if(loadedModels, [modId](const std::pair<std::string, ResourceId> &elem){ return elem.second == modId; });
        }
    }

    ResourceId GLRenderer::CreateFramebuffer(int width, int height, bool useDepth)
    {
        int id = highestUnusedFrambufId++;

        int framebufferWidth = width;
        int framebufferHeight = height;
        if(width == -1)
            framebufferWidth = config.GetValue("r_width", 800);
        if(height == -1)
            framebufferHeight = config.GetValue("r_height", 600);

        //GL_SRGB8_ALPHA8
        //GL_RGBA8

        framebuffers[id] = Framebuffer::Create(framebufferWidth, framebufferHeight, GL_RGBA8, useDepth);

        return id;
    }

    void GLRenderer::DestroyFramebuffer(ResourceId framebufId)
    {
        if(framebufId > 0 && framebuffers.count(framebufId) > 0)
        {
            framebuffers.erase(framebufId);
        }
    }

    void GLRenderer::SetFramebuffer(ResourceId framebufferId)
    {
        if(framebufferId >= 0 && framebuffers.count(framebufferId) > 0)
        {
            Framebuffer &framebuffer = framebuffers.at(framebufferId);
            framebuffer.Bind();
        }
    }

    void GLRenderer::FramebufferResize(ResourceId framebufferId, int width, int height)
    {
        // do not resize the default framebuffer (0)
        if(framebufferId > 0 && framebuffers.count(framebufferId) > 0)
        {
            Framebuffer &framebuffer = framebuffers.at(framebufferId);
            framebuffer.Resize(width, height);
        }
    }

    void GLRenderer::Clear(ResourceId framebufferId, const glm::vec4 &clearColor, float depth)
    {
        Framebuffer &framebuffer = framebuffers.at(framebufferId);
        framebuffer.Clear(clearColor, depth);
    }

    void GLRenderer::Begin(const glm::mat4 &viewProjection)
    {
        perFrameData.viewProjection = viewProjection;
        perFrameBuffer.UpdateData(perFrameData);
    }

    void GLRenderer::Draw(ResourceId modelId, const glm::mat4 &world)
    {
        perObjectData.world = world;
        perObjectBuffer.UpdateData(perObjectData);

        perFrameBuffer.Bind(GL_UNIFORM_BUFFER, 0);
        perObjectBuffer.Bind(GL_UNIFORM_BUFFER, 1);

        Model &model = models.at(modelId);
        model.Bind(modelVAO);
        modelVAO.Bind();

        pipeline.SetShader(staticModelShader);
        pipeline.SetShader(modelUnlitShader);
        pipeline.Use();

        glDrawElements(GL_TRIANGLES, model.NumIndices(), GL_UNSIGNED_SHORT, nullptr);
    }

    void GLRenderer::End()
    {
    }

    void GLRenderer::Draw(ResourceId framebufferId)
    {
        if(framebufferId == DEFAULT_FRAMBUFFER_ID || framebuffers.count(framebufferId) == 0)
            return;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        auto &framebuffer = framebuffers.at(framebufferId);
        framebuffer.GetColorTexture().Bind(0);
        uiSampler.Bind(0);

        postprocessVAO.Bind();

        pipeline.SetShader(ppVertex);
        pipeline.SetShader(ppCopy);
        pipeline.Use();

        SetFramebuffer(DEFAULT_FRAMBUFFER_ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_BLEND);
    }

    void GLRenderer::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
        renderer2d.SetViewsize(width, height);
    }

    I2DRenderer& GLRenderer::Get2DRenderer()
    {
        return renderer2d;
    }
}
