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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include <gli/gli.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <set>

#include <experimental/unordered_map>

#include "misc/Logger.hpp"

#include "graphics/VertexTypes.hpp"
#include "graphics/Image.hpp"
#include "graphics/ShaderSourceBuilder.hpp"
#include "graphics/Transform.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <utf8.h>

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
        namespace GLTFHelper
        {
            template<typename T, std::size_t Size = sizeof T>
            std::vector<T>& VectorReinterpret(const std::vector<unsigned char> &in, std::vector<T> &out)
            {
                if(in.size() % Size != 0) throw std::runtime_error("input size mismatch");
                out.resize(in.size() / Size);
                auto beg = in.cbegin();
                auto end = in.cend();

                std::size_t i = 0;
                for(auto it = beg; it != end;)
                {
                    std::vector<unsigned char> bytes(it, it + Size);
                    T val;
                    std::memcpy(&val, bytes.data(), Size);

                    out[i++] = val;

                    it += Size;
                }

                return out;
            }

            template<typename T, std::size_t Size = sizeof T>
            std::vector<T> VectorReinterpret(const std::vector<unsigned char> &in)
            {
                if(in.size() % Size != 0) throw std::runtime_error("input size mismatch");
                std::vector<T> out(in.size() / Size);
                auto beg = in.cbegin();
                auto end = in.cend();

                std::size_t i = 0;
                for(auto it = beg; it != end;)
                {
                    std::vector<unsigned char> bytes(it, it + Size);
                    T val;
                    std::memcpy(&val, bytes.data(), Size);

                    out[i++] = val;

                    it += Size;
                }

                return out;
            }

            template<typename InType, typename OutType, std::size_t Size = sizeof InType>
            std::vector<OutType>& VectorCast(const std::vector<unsigned char> &in, std::vector<OutType> &out)
            {
                if(in.size() % Size != 0) throw std::runtime_error("input size mismatch");
                out.clear();
                auto beg = in.cbegin();
                auto end = in.cend();

                for(auto it = beg; it != end;)
                {
                    std::vector<unsigned char> bytes(beg, end);
                    InType val;
                    std::memcpy(&val, bytes.data(), Size);

                    out.push_back(static_cast<OutType>(val));
                    it += Size;
                }

                return out;
            }

            template<typename T, std::size_t Size = sizeof T>
            std::vector<T> VectorCast(const std::vector<unsigned char> &in)
            {
                if(in.size() % Size != 0) throw std::runtime_error("input size mismatch");
                std::vector<OutType> out;
                auto beg = in.cbegin();
                auto end = in.cend();

                for(auto it = beg; it != end;)
                {
                    std::vector<unsigned char> bytes(beg, end);
                    InType val;
                    std::memcpy(&val, bytes.data(), Size);

                    out.push_back(static_cast<OutType>(val));
                    it += Size;
                }

                return out;
            }

            std::unordered_map<std::size_t, std::size_t> CreateNodeBoneMap(const tinygltf::Model &model, const tinygltf::Skin &skin)
            {
                std::unordered_map<std::size_t, std::size_t> nodeBoneMap;
                std::size_t boneId = 0;
                for(int nodeId : skin.joints)
                    nodeBoneMap.insert({nodeId, boneId++});
                return nodeBoneMap;
            }

            std::size_t GetCompCount(const tinygltf::Accessor &accessor)
            {
                switch(accessor.type)
                {
                case TINYGLTF_TYPE_MAT4: return 16;
                case TINYGLTF_TYPE_VEC4: return 4;
                case TINYGLTF_TYPE_VEC3: return 3;
                case TINYGLTF_TYPE_VEC2: return 2;
                case TINYGLTF_TYPE_SCALAR: return 1;
                }
                return 0;
            }

            template<typename T, std::size_t Size = sizeof T>
            void GetScalarValues(const tinygltf::Model &model, std::vector<T> &out, std::size_t compCount, const tinygltf::Accessor &accessor)
            {
                out.resize(accessor.count * compCount);
                const auto &bufferView = model.bufferViews.at(accessor.bufferView);
                const auto &buffer = model.buffers.at(bufferView.buffer);
                auto beg = buffer.data.cbegin() + bufferView.byteOffset + accessor.byteOffset;
                auto end = buffer.data.cbegin() + bufferView.byteOffset + accessor.byteOffset + bufferView.byteLength;
                
                std::size_t i = 0;
                for(auto it = beg; it != end;)
                {
                    std::vector<unsigned char> bytes(it, it + Size);
                    T val;
                    std::memcpy(&val, bytes.data(), Size);

                    out[i++] = val;

                    it += Size;
                }
            };

            template<typename T, std::size_t Size = sizeof T>
            void GetValues(const tinygltf::Model &model, const tinygltf::Accessor &accessor, std::vector<T> &out)
            {
                out.resize(accessor.count);
                const auto &bufferView = model.bufferViews.at(accessor.bufferView);
                const auto &buffer = model.buffers.at(bufferView.buffer);

                auto beg = buffer.data.cbegin() + bufferView.byteOffset + accessor.byteOffset;
                auto end = buffer.data.cbegin() + bufferView.byteOffset + accessor.byteOffset + bufferView.byteLength;

                std::size_t i = 0;
                for(auto it = beg; it != end;)
                {
                    std::vector<unsigned char> bytes(it, it + Size);
                    T val;
                    std::memcpy(&val, bytes.data(), Size);

                    out[i++] = val;

                    it += Size;
                }
            }

            template<typename T, unsigned int N>
            static void TrackFromChannel(const tinygltf::Model &model, Animation::Track<T, N> &result, const tinygltf::AnimationChannel &channel, const tinygltf::Animation &animation)
            {
                const tinygltf::AnimationSampler &sampler = animation.samplers.at(channel.sampler);
                Animation::Interpolation interpolation = Animation::Interpolation::CONSTANT;
                if(sampler.interpolation == "LINEAR")
                    interpolation = Animation::Interpolation::LINEAR;
                else if(sampler.interpolation == "CUBICSPLINE")
                    interpolation = Animation::Interpolation::CUBIC;

                bool isSamplerCubic = interpolation == Animation::Interpolation::CUBIC;
                result.SetInterpolation(interpolation);

                const auto &inAccessor = model.accessors.at(sampler.input);
                const auto &outAccessor = model.accessors.at(sampler.output);

                std::vector<float> time;
                auto compCount = GLTFHelper::GetCompCount(inAccessor);
                GetScalarValues(model, time, compCount, inAccessor);

                std::vector<float> val;
                compCount = GLTFHelper::GetCompCount(outAccessor);
                GetScalarValues(model, val, compCount, outAccessor);

                if(compCount != N) throw std::runtime_error("compCount mismatch");

                std::size_t numFrames = inAccessor.count;
                result.Resize(numFrames);

                for(std::size_t i = 0; i < numFrames; ++i)
                {
                    std::size_t baseIndex = i * compCount;
                    Animation::Frame<N> &frame = result[i];
                    std::size_t offset = 0;

                    frame.time = time.at(i);

                    for(std::size_t comp = 0; comp < compCount; ++comp)
                        frame.in[comp] = isSamplerCubic ? val.at(baseIndex + offset++) : 0.0f;
                    for(std::size_t comp = 0; comp < compCount; ++comp)
                        frame.value[comp] = val.at(baseIndex + offset++);
                    for(std::size_t comp = 0; comp < compCount; ++comp)
                        frame.out[comp] = isSamplerCubic ? val.at(baseIndex + offset++) : 0.0f;
                }
            }

            Animation::Pose LoadRestPose(const tinygltf::Model &model, const tinygltf::Skin &skin, std::vector<std::string> &jointNames, Logger &logger)
            {
                auto nodeToTransform = [](const tinygltf::Node &node)
                {
                    Transform transform;
                    if(node.matrix.size() > 0)
                    {
                        glm::mat4 matrix = glm::make_mat4(node.matrix.data());
                        transform = Mat4ToTransform(matrix);
                    }
                    if(node.translation.size() > 0)
                        transform.position = glm::make_vec3(node.translation.data());
                    if(node.scale.size() > 0)
                        transform.scale = glm::make_vec3(node.scale.data());
                    if(node.rotation.size() > 0)
                        transform.rotation = glm::make_quat(node.rotation.data());
                    return transform;
                };

                const auto &nodeBoneMap = GLTFHelper::CreateNodeBoneMap(model, skin);

                std::size_t boneCount = skin.joints.size();
                Animation::Pose pose(boneCount);
                const auto &joints = skin.joints;
                std::vector<std::string> names(boneCount, "No Name");

                auto getParent = [&joints, &model](int nodeIndex)
                {
                    for(int nodeId : joints)
                    {
                        const auto &node = model.nodes.at(nodeId);
                        if(std::count(node.children.begin(), node.children.end(), nodeIndex) > 0)
                            return nodeId;
                    }
                    return -1;
                };

                for(int i : joints)
                {
                    std::size_t boneId = nodeBoneMap.at(i);
                    const auto &node = model.nodes.at(i);
                    Transform t = nodeToTransform(node);
                    pose.SetLocalTransform(boneId, t);
                    int parent = getParent(i);

                    int parentBoneId = parent == -1 ? -1 : nodeBoneMap.at(parent);
                    pose.SetParent(boneId, parentBoneId);
                    
                    names[boneId] = node.name;
                }

                jointNames = names;
                return pose;
            }

            Animation::Pose LoadBindPose(const tinygltf::Model &model, const tinygltf::Skin &skin, Animation::Pose &restPose, Logger &logger)
            {
                std::size_t numBones = restPose.Size();
                std::vector<Transform> worldBindPose(numBones);
                for(std::size_t i = 0; i < numBones; ++i)
                    worldBindPose[i] = restPose.GetGlobalTransform(i);

                const tinygltf::Accessor &matAccessor = model.accessors.at(skin.inverseBindMatrices);

                std::vector<float> invBindAccessor;
                auto compCount = GLTFHelper::GetCompCount(matAccessor);

                if(compCount != 16) throw std::runtime_error("compCount mismatch");

                GLTFHelper::GetScalarValues(model, invBindAccessor, compCount, matAccessor);

                std::size_t numJoints = skin.joints.size();
                for(std::size_t i = 0; i < numJoints; ++i)
                {
                    std::size_t boneId = i;
                    float *matrix = invBindAccessor.data() + i * compCount;
                    glm::mat4 invBindMatrix = glm::make_mat4(matrix);
                    glm::mat4 bindMatrix = glm::inverse(invBindMatrix);
                    Transform bindTransform = Mat4ToTransform(bindMatrix);
                    worldBindPose[boneId] = bindTransform;
                }

                Animation::Pose bindPose(restPose);
                for(std::size_t i = 0; i < numBones; ++i)
                {
                    Transform current = worldBindPose.at(i);
                    int parentId = bindPose.GetParent(i);
                    if(parentId >= 0)
                    {
                        Transform parent = worldBindPose.at(parentId);
                        current = Combine(Inverse(parent), current);
                    }
                    bindPose.SetLocalTransform(i, current);
                }

                return bindPose;
            }
        }

        struct NoSkinException : public RISException
        {
            NoSkinException(const std::string &reason) : RISException(reason) {}
        };

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

            Config &config = GetConfig();

            int msaa = config.GetValue("r_msaa", 0);
            if(msaa > 0)
                glEnable(GL_MULTISAMPLE);

            log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);

            int width = config.GetValue("r_width", 800);
            int height = config.GetValue("r_height", 600);
            Resize(width, height);

            loadedTextures.insert({"__DEFAULT_TEX", std::make_shared<Texture>(glm::vec4(1, 1, 1, 1))});
            loadedTextures.insert({"__MISSING_TEX", std::make_shared<Texture>(glm::vec4(1, 0, 1, 1))});

            loadedMeshes.insert({"__MISSING_MESH", std::make_shared<Mesh>(Buffer(glm::vec3(), GL_DYNAMIC_STORAGE_BIT), Buffer(glm::vec3(), GL_DYNAMIC_STORAGE_BIT), Buffer(glm::vec2(), GL_DYNAMIC_STORAGE_BIT), Buffer(glm::i16vec4(), GL_DYNAMIC_STORAGE_BIT), Buffer(glm::vec4(), GL_DYNAMIC_STORAGE_BIT), Buffer((uint16_t)0, GL_DYNAMIC_STORAGE_BIT), 1)});
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

                std::unordered_map<uint32_t, Glyph> glyphs;

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
                    glyph.s0 = glyphJson["s0"].GetFloat();
                    glyph.t0 = glyphJson["t0"].GetFloat();
                    glyph.s1 = glyphJson["s1"].GetFloat();
                    glyph.t1 = glyphJson["t1"].GetFloat();

                    auto c = glyphJson["charcode"].GetString();
                    auto cc = std::string(c);
                    glyph.charCode = utf8::next(cc.begin(), cc.end());

                    const rapidjson::Value &kerningsJson = glyphJson["kernings"];
                    for(auto kItr = kerningsJson.MemberBegin(); kItr != kerningsJson.MemberEnd(); ++kItr)
                    {
                        auto cc = std::string(kItr->name.GetString());
                        uint32_t c = utf8::next(cc.begin(), cc.end());
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
                    const auto &nodeBoneMap = GLTFHelper::CreateNodeBoneMap(model, model.skins.at(0));

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

                    auto interleave = [](const std::vector<std::pair<std::size_t, std::vector<unsigned char>>> &buffers, std::size_t numElements)
                    {
                        std::vector<unsigned char> buffer;

                        for(std::size_t i = 0; i < numElements; ++i)
                        {
                            for(const auto &buf : buffers)
                            {
                                std::size_t stride = buf.first;
                                std::size_t offset = stride * i;
                                const auto &b = buf.second;
                                buffer.insert(buffer.end(), b.cbegin() + offset, b.cbegin() + offset + stride);
                            }
                        }

                        return buffer;
                    };

                    const tinygltf::Mesh &mesh = model.meshes.at(0);
                    const tinygltf::Primitive &primitive = mesh.primitives.at(0);

                    if(!checkAttribs(primitive.attributes))
                        throw RISException("model in unsupported format");

                    auto fillBuffer = [&model, &primitive](std::vector<unsigned char> &buf, int attribIndex)
                    {
                        const auto &accessor = model.accessors.at(attribIndex);
                        const auto &bufferView = model.bufferViews.at(accessor.bufferView);
                        const auto &buffer = model.buffers.at(bufferView.buffer);

                        auto beg = buffer.data.cbegin() + accessor.byteOffset + bufferView.byteOffset;
                        auto end = buffer.data.cbegin() + accessor.byteOffset + bufferView.byteOffset + bufferView.byteLength;

                        buf.clear();
                        buf.insert(buf.begin(), beg, end);
                    };

                    std::size_t numElements = model.accessors.at(primitive.attributes.at("POSITION")).count;

                    /*
                    std::vector<std::vector<unsigned char>> buffers(6);
                    fillBuffer(buffers[0], primitive.attributes.at("POSITION"));
                    fillBuffer(buffers[1], primitive.attributes.at("NORMAL"));
                    fillBuffer(buffers[2], primitive.attributes.at("TEXCOORD_0"));
                    fillBuffer(buffers[3], primitive.indices);
                    if(primitive.attributes.count("JOINTS_0"))
                    {
                        fillBuffer(buffers[4], primitive.attributes.at("JOINTS_0"));
                        fillBuffer(buffers[5], primitive.attributes.at("WEIGHTS_0"));
                    }
                    else
                    {
                        buffers[4] = std::vector<unsigned char>(numElements * sizeof glm::i16vec4, 0);
                        buffers[5] = std::vector<unsigned char>(numElements * sizeof glm::vec4, 0);
                    }

                    auto buf = interleave({{sizeof glm::vec3, buffers.at(0)}, 
                                            {sizeof glm::vec3, buffers.at(1)},
                                            {sizeof glm::vec2, buffers.at(2)},
                                            {sizeof glm::i16vec4, buffers.at(4)},
                                            {sizeof glm::vec4, buffers.at(5)}},
                                            numElements);

                    auto j = GLTFHelper::VectorReinterpret<glm::i16vec4>(buffers.at(4));

                    auto bufCast = GLTFHelper::VectorReinterpret<VertexType::ModelVertex>(buf);
                    */

                    std::vector<glm::vec3> positions;
                    const auto &posAccessor = model.accessors.at(primitive.attributes.at("POSITION"));
                    GLTFHelper::GetValues(model, posAccessor, positions);

                    std::vector<glm::vec3> normals;
                    const auto &normAccessor = model.accessors.at(primitive.attributes.at("NORMAL"));
                    GLTFHelper::GetValues(model, normAccessor, normals);

                    std::vector<glm::vec2> texCoords;
                    const auto &texAccessor = model.accessors.at(primitive.attributes.at("TEXCOORD_0"));
                    GLTFHelper::GetValues(model, texAccessor, texCoords);

                    std::vector<glm::i16vec4> joints;
                    if(primitive.attributes.count("JOINTS_0"))
                    {
                        const auto &jointAccessor = model.accessors.at(primitive.attributes.at("JOINTS_0"));
                        GLTFHelper::GetValues(model, jointAccessor, joints);
                    }
                    else
                    {
                        joints.resize(numElements);
                    }

                    std::vector<glm::vec4> weights;
                    if(primitive.attributes.count("WEIGHTS_0"))
                    {
                        const auto &weightAccessor = model.accessors.at(primitive.attributes.at("WEIGHTS_0"));
                        GLTFHelper::GetValues(model, weightAccessor, weights);
                    }
                    else
                    {
                        weights.resize(numElements);
                    }

                    std::vector<unsigned short> indices;
                    const tinygltf::Accessor &indexAccessor = model.accessors.at(primitive.indices);
                    GLTFHelper::GetValues(model, indexAccessor, indices);

                    Buffer positionBuffer(positions, 0);
                    Buffer normalBuffer(normals, 0);
                    Buffer texCoordBuffer(texCoords, 0);
                    Buffer jointsBuffer(joints, 0);
                    Buffer weightsBuffer(weights, 0);
                    Buffer indexBuffer(indices, 0);
                    
                    int numIndices = model.accessors.at(primitive.indices).count;

                    std::shared_ptr<Mesh> m = std::make_shared<Mesh>(std::move(positionBuffer), std::move(normalBuffer), std::move(texCoordBuffer), std::move(jointsBuffer), std::move(weightsBuffer), std::move(indexBuffer), numIndices);
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

            loader.RegisterLoadFunction<Animation::Skeleton>([this, &logger](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedSkeletons.count(name) > 0)
                {
                    return loadedSkeletons.at(name);
                }

                tinygltf::TinyGLTF gltfLoader;
                tinygltf::Model model;
                std::string err, warn;

                bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(data.data()), data.size());
                if(!warn.empty()) logger.Warning(warn);
                if(result)
                {
                    if(model.skins.size() == 0)
                    {
                        logger.Warning("no skin found");
                        throw NoSkinException("no skin");
                    }

                    if(model.skins.size() > 1)
                    {
                        logger.Warning("more than 1 skin found. using the first one");
                    }

                    const auto &skin = model.skins.at(0);

                    std::vector<std::string> names;
                    Animation::Pose restPose = GLTFHelper::LoadRestPose(model, skin, names, logger);
                    Animation::Pose bindPose = GLTFHelper::LoadBindPose(model, skin, restPose, logger);

                    std::shared_ptr<Animation::Skeleton> skeleton = std::make_shared<Animation::Skeleton>(std::move(restPose), std::move(bindPose), std::move(names));
                    if(cache)
                        loadedSkeletons.insert_or_assign(name, skeleton);
                    return skeleton;
                }
                else
                {
                    if(!err.empty()) logger.Error(err);
                    throw RISException("skeleton error"); // make better error
                }
            });

            loader.RegisterLoadFunction<Animation::Animation>([this, &logger](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedAnimations.count(name) > 0)
                {
                    return loadedAnimations.at(name);
                }

                tinygltf::TinyGLTF gltfLoader;
                tinygltf::Model model;
                std::string err, warn;

                bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(data.data()), data.size());
                if(!warn.empty()) logger.Warning(warn);
                if(result)
                {
                    std::size_t numClips = model.animations.size();

                    std::vector<Animation::Clip> clips(numClips);

                    const auto &nodeBoneMap = GLTFHelper::CreateNodeBoneMap(model, model.skins.at(0));

                    for(std::size_t i = 0; i < numClips; ++i)
                    {
                        const tinygltf::Animation &animation = model.animations.at(i);
                        Animation::Clip &clip = clips[i];
                        clip.SetName(animation.name);

                        std::size_t numChannels = animation.channels.size();
                        for(const auto &channel : animation.channels)
                        {
                            const tinygltf::Node &targetNode = model.nodes.at(channel.target_node);
                            int nodeId = nodeBoneMap.at(channel.target_node);
                            if(channel.target_path == "translation")
                            {
                                auto &track = clip[nodeId].GetPositionTrack();
                                GLTFHelper::TrackFromChannel(model, track, channel, animation);
                            }
                            else if(channel.target_path == "scale")
                            {
                                auto &track = clip[nodeId].GetScaleTrack();
                                GLTFHelper::TrackFromChannel(model, track, channel, animation);
                            }
                            else if(channel.target_path == "rotation")
                            {
                                auto &track = clip[nodeId].GetRotationTrack();
                                GLTFHelper::TrackFromChannel(model, track, channel, animation);
                            }
                        }

                        clip.RecalculateDuration();
                    }

                    std::shared_ptr<Animation::Animation> animation = std::make_shared<Animation::Animation>(std::move(clips));
                    if(cache)
                        loadedAnimations.insert_or_assign(name, animation);
                    return animation;
                }
                else
                {
                    if(!err.empty()) logger.Error(err);
                    throw RISException("animation error"); // make better error
                }
            });

            loader.RegisterLoadFunction<Model>([this, &loader](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                std::string modelStr(reinterpret_cast<const char*>(data.data()), data.size());

                rapidjson::Document modelJson;
                rapidjson::ParseResult res = modelJson.Parse(modelStr.c_str()); 
                if(res.IsError())
                {
                    std::string errorMsg = rapidjson::GetParseError_En(res.Code());
                    Logger::Instance().Error("Failed to parse model (" + name + "): " + errorMsg + "(" + std::to_string(res.Offset()) + ")");
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

        void Renderer::ClearCacheFonts()
        {
            loadedFonts.clear();
        }

        void Renderer::ClearCacheSkeletons()
        {
            loadedSkeletons.clear();
        }

        void Renderer::ClearCacheAnimations()
        {
            loadedAnimations.clear();
        }

        void Renderer::ClearCache()
        {
            ClearCacheTextures();
            ClearCacheMeshes();
            ClearCacheModels();
            ClearCacheShaders();
            ClearCacheFonts();
            ClearCacheSkeletons();
            ClearCacheAnimations();
        }

        void Renderer::Resize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    }
}
