#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#include <tiny_gltf.h>

#include "graphics/Renderer.hpp"

#include "RIS.hpp"

#include "loader/Loader.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <gli/gli.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include <experimental/unordered_map>

#include "misc/Logger.hpp"

#include "graphics/VertexTypes.hpp"
#include "graphics/Image.hpp"
#include "graphics/ShaderSourceBuilder.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

using namespace std::literals;

// Windows "hack" to force some laptops to use the highperformance GPU
// some intel integrated gpus don't support opengl 4.5
// I might, in the future, reduce the required opengl version
#ifdef _WIN32
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace RIS
{
    namespace Graphics
    {
        Renderer::Renderer()
        {
            auto &log = Logger::Instance();

            if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            {
                throw RendererException("Couldn't load GL functions");
            }

            if(!GLAD_GL_VERSION_4_5)
            {
                log.Error("OpenGL version 4.5 not supported");
                throw RendererException("OpenGL version 4.5 or higher not supported");
            }

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

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
            std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
            std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
            std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

            if(vendor.find("ATI") != std::string::npos || vendor.find("ati") != std::string::npos)
                useAmdFix = true;

            Config &config = GetConfig();

            if(const_cast<const Config&>(config).GetValue("g_ignoreamdfix", false))
                useAmdFix = false;

            log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);

            int width = config.GetValue("r_width", 800);
            int height = config.GetValue("r_height", 600);
            Resize(width, height);

            loadedTextures.insert({"__DEFAULT_TEX", std::make_shared<Texture>(glm::vec4(1, 1, 1, 1))});
            loadedTextures.insert({"__MISSING_TEX", std::make_shared<Texture>(glm::vec4(1, 0, 1, 1))});

            loadedMeshes.insert({"__MISSING_MESH", std::make_shared<Mesh>(Buffer(VertexType::ModelVertex(), GL_DYNAMIC_STORAGE_BIT), Buffer((uint16_t)0, GL_DYNAMIC_STORAGE_BIT), 1)});
        }

        void Renderer::PostInit()
        {
            auto &loader = GetLoader();

            loader.RegisterLoadFunction<Texture>([this](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedTextures.count(name) > 0)
                {
                    return loadedTextures.at(name);
                }
                auto texture = std::make_shared<Texture>(data);
                if(cache)
                    loadedTextures.insert_or_assign(name, texture);
                return texture;
            }, [this]()
            {
                return loadedTextures.at("__MISSING_TEX");
            });

            loader.RegisterLoadFunction<Shader>([this, &loader](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedShaders.count(name) > 0)
                {
                    return loadedShaders.at(name);
                }

                GLenum shaderType;
                auto shaderParam = std::any_cast<ShaderType>(param);
                if(shaderParam == ShaderType::VERTEX)
                    shaderType = GL_VERTEX_SHADER;
                else if(shaderParam == ShaderType::FRAGMENT)
                    shaderType = GL_FRAGMENT_SHADER;

                std::string path = std::filesystem::path(name).remove_filename().generic_string();

                ShaderSourceBuilder builder;

                std::string shaderSrc(reinterpret_cast<const char*>(data.data()), data.size());
                std::string shaderSrcProcessed = builder.BuildSource(shaderSrc, name, [&loader, &path](const std::string &fileName)
                {
                    std::string file = path + fileName;
                    return *loader.Load<std::string>(file);
                });

                std::shared_ptr<Shader> shader;
                shader = std::make_shared<Shader>(shaderSrcProcessed, shaderType);

                if(cache)
                    loadedShaders.insert_or_assign(name, shader);
                return shader;
            });

            loader.RegisterLoadFunction<Image>([](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                return std::make_shared<Image>(data);
            });

            loader.RegisterLoadFunction<Font>([this, &loader](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedFonts.count(name) > 0)
                {
                    return loadedFonts.at(name);
                }

                std::string fontStr(reinterpret_cast<const char*>(data.data()), data.size());

                rapidjson::Document fontJson;
                rapidjson::ParseResult res = fontJson.Parse(fontStr.c_str()); 
                if(res.IsError())
                {
                    std::string errorMsg = rapidjson::GetParseError_En(res.Code());
                    Logger::Instance().Error("Failed to parse font ("s + name + "): "s + errorMsg + "(" + std::to_string(res.Offset()) + ")");
                    throw RISException("font error"); //make better error
                }

                float ascender = fontJson["ascender"].GetFloat();
                float bitmapWidth = fontJson["bitmap_width"].GetInt();
                float bitmapHeight = fontJson["bitmap_height"].GetInt();
                float descender = fontJson["descender"].GetFloat();
                float height = fontJson["height"].GetFloat();
                float maxAdvance = fontJson["max_advance"].GetFloat();
                std::string fontName = fontJson["name"].GetString();
                float size = fontJson["size"].GetFloat();
                float spaceAdvance = fontJson["space_advance"].GetFloat();

                std::string fontTexturePath = "fonts/" + fontName + ".dds";
                auto fontTexture = loader.Load<Texture>(fontTexturePath);

                std::unordered_map<char, Glyph> glyphs;

                const rapidjson::Value &glyphData = fontJson["glyph_data"];
                for (auto itr = glyphData.MemberBegin(); itr != glyphData.MemberEnd(); ++itr)
                {
                    const rapidjson::Value &glyphJson = itr->value;
                    Glyph glyph = { 0 };
                    glyph.advanceX = glyphJson["advance_x"].GetFloat();
                    glyph.bboxHeight = glyphJson["bbox_height"].GetFloat();
                    glyph.bboxWidth = glyphJson["bbox_width"].GetFloat();
                    glyph.bearingX = glyphJson["bearing_x"].GetFloat();
                    glyph.bearingY = glyphJson["bearing_y"].GetFloat();
                    glyph.charCode = glyphJson["charcode"].GetString()[0];
                    glyph.s0 = glyphJson["s0"].GetFloat();
                    glyph.t0 = glyphJson["t0"].GetFloat();
                    glyph.s1 = glyphJson["s1"].GetFloat();
                    glyph.t1 = glyphJson["t1"].GetFloat();

                    const rapidjson::Value &kerningsJson = glyphJson["kernings"];
                    for(auto kItr = kerningsJson.MemberBegin(); kItr != kerningsJson.MemberEnd(); ++kItr)
                    {
                        char c = kItr->name.GetString()[0];
                        float val = kItr->value.GetFloat();
                        glyph.kernings.insert({ c, val });
                    }

                    glyphs.insert({ glyph.charCode, glyph });
                }

                auto font = std::make_shared<Font>(ascender, descender, height, maxAdvance, fontName, size, spaceAdvance, glyphs, fontTexture);
                if(cache)
                    loadedFonts.insert_or_assign(name, font);
                return font;
            });

            Logger &logger = Logger::Instance();

            loader.RegisterLoadFunction<Mesh>([this, &loader, &logger](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedMeshes.count(name) > 0)
                {
                    return loadedMeshes.at(name);
                }

                tinygltf::TinyGLTF gltfLoader;
                tinygltf::Model model;
                std::string err, warn;

                bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(data.data()), data.size());
                if(!warn.empty()) logger.Warning(warn);
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

                    auto interleave = [](const std::vector<std::pair<size_t, std::vector<unsigned char>>> &buffers, size_t numElements)
                    {
                        std::vector<unsigned char> buffer;

                        for(size_t i = 0; i < numElements; ++i)
                        {
                            for(const auto &buf : buffers)
                            {
                                size_t stride = buf.first;
                                size_t offset = stride * i;
                                const auto &b = buf.second;
                                buffer.insert(buffer.end(), b.begin() + offset, b.begin() + offset + stride);
                            }
                        }

                        return buffer;
                    };

                    const tinygltf::Mesh &mesh = model.meshes.at(0);
                    const tinygltf::Primitive &primitive = mesh.primitives.at(0);

                    if(!checkAttribs(primitive.attributes))
                        throw std::exception("model in wrong format");

                    std::vector<std::vector<unsigned char>> buffers;

                    size_t index = 0;
                    for(const tinygltf::BufferView &bufferView : model.bufferViews)
                    {
                        const tinygltf::Buffer &buffer = model.buffers.at(bufferView.buffer);
                        auto begin = buffer.data.begin() + bufferView.byteOffset;
                        auto end = buffer.data.begin() + bufferView.byteOffset + bufferView.byteLength;

                        buffers.push_back(std::vector(begin, end));

                        index++;
                    }

                    int indexBufferIndex = primitive.indices;
                    int positionIndex = primitive.attributes.at("POSITION");
                    int normalIndex = primitive.attributes.at("NORMAL");
                    int texcoordIndex = primitive.attributes.at("TEXCOORD_0");
                    int jointIndex;
                    int weightIndex;

                    size_t numElements = buffers.at(positionIndex).size() / sizeof glm::vec3;

                    if(primitive.attributes.count("JOINTS_0"))
                    {
                        jointIndex = primitive.attributes.at("JOINTS_0");
                        weightIndex = primitive.attributes.at("WEIGHTS_0");
                    }
                    else
                    {
                        jointIndex = buffers.size();
                        buffers.push_back(std::vector<unsigned char>(numElements * sizeof glm::i8vec4, 0));
                        weightIndex = jointIndex+1;
                        buffers.push_back(std::vector<unsigned char>(numElements * sizeof glm::vec4, 0));
                    }

                    Buffer vertexBuffer(interleave({{sizeof glm::vec3, buffers.at(positionIndex)}, 
                                                    {sizeof glm::vec3, buffers.at(normalIndex)  },
                                                    {sizeof glm::vec2, buffers.at(texcoordIndex)},
                                                    {sizeof glm::i8vec4, buffers.at(jointIndex) },
                                                    {sizeof glm::vec4, buffers.at(weightIndex)  }},
                                                    numElements),
                                        GL_DYNAMIC_STORAGE_BIT);
                    Buffer indexBuffer(buffers.at(indexBufferIndex), GL_DYNAMIC_STORAGE_BIT);
                    
                    int numIndices = model.accessors.at(indexBufferIndex).count;

                    std::shared_ptr<Mesh> m = std::make_shared<Mesh>(std::move(vertexBuffer), std::move(indexBuffer), numIndices);
                    if(cache)
                        loadedMeshes.insert_or_assign(name, m);
                    return m;
                }
                else
                {
                    if(!err.empty()) logger.Error(err);
                    throw RISException("mesh error"); // make better error
                }
            }, [this]()
            {
                return loadedMeshes.at("__MISSING_MESH");
            });

            loader.RegisterLoadFunction<Model>([this, &loader](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                std::string modelStr(reinterpret_cast<const char*>(data.data()), data.size());

                rapidjson::Document modelJson;
                rapidjson::ParseResult res = modelJson.Parse(modelStr.c_str()); 
                if(res.IsError())
                {
                    std::string errorMsg = rapidjson::GetParseError_En(res.Code());
                    Logger::Instance().Error("Failed to parse font (" + name + "): " + errorMsg + "(" + std::to_string(res.Offset()) + ")");
                    throw RISException("model error"); // make better error
                }

                std::string meshName = modelJson["mesh"].GetString();
                std::string textureName = modelJson["texture"].GetString();

                auto textureId = loader.Load<Texture>(textureName);
                auto meshId = loader.Load<Mesh>(meshName);

                std::shared_ptr<Model> model = std::make_shared<Model>(meshId, textureId);
                if(cache)
                    loadedModels.insert_or_assign(name, model);
                return model;
            });
        }

        void Renderer::ClearCacheTextures()
        {
            loadedTextures.clear();
        }

        void Renderer::ClearCacheMeshes()
        {
            loadedMeshes.clear();
        }

        void Renderer::ClearCacheModels()
        {
            loadedModels.clear();
        }

        void Renderer::ClearCacheShaders()
        {
            loadedShaders.clear();
        }

        void Renderer::ClearCache()
        {
            ClearCacheTextures();
            ClearCacheMeshes();
            ClearCacheModels();
            ClearCacheShaders();
        }

        void Renderer::Resize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    }
}
