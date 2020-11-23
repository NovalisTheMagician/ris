#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#include <tiny_gltf.h>

#include "graphics/Model.hpp"

#include "misc/Logger.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <type_traits>

namespace RIS
{
    namespace Graphics
    {
        Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
            : mesh(mesh), texture(texture)
        {}

        std::shared_ptr<Mesh> Model::GetMesh()
        {
            return mesh;
        }

        std::shared_ptr<Texture> Model::GetTexture()
        {
            return texture;
        }
    }

    namespace GLTFHelper
    {
        template<typename T, std::size_t Size = sizeof T>
        static std::vector<T>& VectorReinterpret(const std::vector<unsigned char> &in, std::vector<T> &out)
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
        static std::vector<T> VectorReinterpret(const std::vector<unsigned char> &in)
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
        static std::vector<OutType>& VectorCast(const std::vector<unsigned char> &in, std::vector<OutType> &out)
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

        template<typename InType, typename OutType, std::size_t Size = sizeof InType>
        static std::vector<OutType> VectorCast(const std::vector<unsigned char> &in)
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

        static std::unordered_map<std::size_t, std::size_t> CreateNodeBoneMap(const tinygltf::Model &model, const tinygltf::Skin &skin)
        {
            std::unordered_map<std::size_t, std::size_t> nodeBoneMap;
            std::size_t boneId = 0;
            for(int nodeId : skin.joints)
                nodeBoneMap.insert({nodeId, boneId++});
            return nodeBoneMap;
        }

        static std::size_t GetCompCount(const tinygltf::Accessor &accessor)
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
        static void GetScalarValues(const tinygltf::Model &model, std::vector<T> &out, std::size_t compCount, const tinygltf::Accessor &accessor)
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
        static void GetValues(const tinygltf::Model &model, const tinygltf::Accessor &accessor, std::vector<T> &out)
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
        static void TrackFromChannel(const tinygltf::Model &model, RIS::Graphics::Animation::Track<T, N> &result, const tinygltf::AnimationChannel &channel, const tinygltf::Animation &animation)
        {
            const tinygltf::AnimationSampler &sampler = animation.samplers.at(channel.sampler);
            RIS::Graphics::Animation::Interpolation interpolation = RIS::Graphics::Animation::Interpolation::CONSTANT;
            if(sampler.interpolation == "LINEAR")
                interpolation = RIS::Graphics::Animation::Interpolation::LINEAR;
            else if(sampler.interpolation == "CUBICSPLINE")
                interpolation = RIS::Graphics::Animation::Interpolation::CUBIC;

            bool isSamplerCubic = interpolation == RIS::Graphics::Animation::Interpolation::CUBIC;
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
                RIS::Graphics::Animation::Frame<N> &frame = result[i];
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

        static RIS::Graphics::Animation::Pose LoadRestPose(const tinygltf::Model &model, const tinygltf::Skin &skin, std::vector<std::string> &jointNames, RIS::Logger &logger)
        {
            auto nodeToTransform = [](const tinygltf::Node &node)
            {
                RIS::Graphics::Transform transform;
                if(node.matrix.size() > 0)
                {
                    glm::mat4 matrix = glm::make_mat4(node.matrix.data());
                    transform = RIS::Graphics::Mat4ToTransform(matrix);
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
            RIS::Graphics::Animation::Pose pose(boneCount);
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
                RIS::Graphics::Transform t = nodeToTransform(node);
                pose.SetLocalTransform(boneId, t);
                int parent = getParent(i);

                int parentBoneId = parent == -1 ? -1 : nodeBoneMap.at(parent);
                pose.SetParent(boneId, parentBoneId);
                
                names[boneId] = node.name;
            }

            jointNames = names;
            return pose;
        }

        static RIS::Graphics::Animation::Pose LoadBindPose(const tinygltf::Model &model, const tinygltf::Skin &skin, RIS::Graphics::Animation::Pose &restPose, RIS::Logger &logger)
        {
            std::size_t numBones = restPose.Size();
            std::vector<RIS::Graphics::Transform> worldBindPose(numBones);
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
                float *matrix = invBindAccessor.data() + i * compCount;
                glm::mat4 invBindMatrix = glm::make_mat4(matrix);
                glm::mat4 bindMatrix = glm::inverse(invBindMatrix);
                RIS::Graphics::Transform bindTransform = RIS::Graphics::Mat4ToTransform(bindMatrix);
                worldBindPose[i] = bindTransform;
            }

            RIS::Graphics::Animation::Pose bindPose(restPose);
            for(std::size_t i = 0; i < numBones; ++i)
            {
                RIS::Graphics::Transform current = worldBindPose.at(i);
                int parentId = bindPose.GetParent(i);
                if(parentId >= 0)
                {
                    RIS::Graphics::Transform parent = worldBindPose.at(parentId);
                    current = Combine(Inverse(parent), current);
                }
                bindPose.SetLocalTransform(i, current);
            }

            return bindPose;
        }
    }

    namespace Loader
    {
        template<>
        std::shared_ptr<Graphics::Mesh> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
        {
            auto &logger = Logger::Instance();

            tinygltf::TinyGLTF gltfLoader;
            tinygltf::Model model;
            std::string err, warn;

            bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(bytes.data()), bytes.size());
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
                    return nullptr;

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

                Graphics::Buffer positionBuffer(positions, 0);
                Graphics::Buffer normalBuffer(normals, 0);
                Graphics::Buffer texCoordBuffer(texCoords, 0);
                Graphics::Buffer jointsBuffer(joints, 0);
                Graphics::Buffer weightsBuffer(weights, 0);
                Graphics::Buffer indexBuffer(indices, 0);
                
                int numIndices = model.accessors.at(primitive.indices).count;

                return std::make_shared<Graphics::Mesh>(std::move(positionBuffer), std::move(normalBuffer), std::move(texCoordBuffer), std::move(jointsBuffer), std::move(weightsBuffer), std::move(indexBuffer), numIndices);
            }
            else
            {
                return nullptr;
            }
        }

        template<>
        std::shared_ptr<Graphics::Animation::Skeleton> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
        {
            auto &logger = Logger::Instance();

            tinygltf::TinyGLTF gltfLoader;
            tinygltf::Model model;
            std::string err, warn;

            bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(bytes.data()), bytes.size());
            if(!warn.empty()) logger.Warning(warn);
            if(result)
            {
                if(model.skins.size() == 0)
                {
                    logger.Warning("no skin found");
                    return nullptr;
                }

                if(model.skins.size() > 1)
                {
                    logger.Warning("more than 1 skin found. using the first one");
                }

                const auto &skin = model.skins.at(0);

                std::vector<std::string> names;
                Graphics::Animation::Pose restPose = GLTFHelper::LoadRestPose(model, skin, names, logger);
                Graphics::Animation::Pose bindPose = GLTFHelper::LoadBindPose(model, skin, restPose, logger);

                return std::make_shared<Graphics::Animation::Skeleton>(std::move(restPose), std::move(bindPose), std::move(names));
            }
            else
            {
                return nullptr;
            }
        }

        template<>
        std::shared_ptr<Graphics::Animation::Animation> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
        {
            auto &logger = Logger::Instance();

            tinygltf::TinyGLTF gltfLoader;
            tinygltf::Model model;
            std::string err, warn;

            bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(bytes.data()), bytes.size());
            if(!warn.empty()) logger.Warning(warn);
            if(result)
            {
                std::size_t numClips = model.animations.size();

                std::vector<Graphics::Animation::Clip> clips(numClips);

                const auto &nodeBoneMap = GLTFHelper::CreateNodeBoneMap(model, model.skins.at(0));

                for(std::size_t i = 0; i < numClips; ++i)
                {
                    const tinygltf::Animation &animation = model.animations.at(i);
                    Graphics::Animation::Clip &clip = clips[i];
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

                if constexpr (std::is_same_v<Graphics::Animation::Animation::ClipType, Graphics::Animation::FastClip>)
                {
                    std::vector<Graphics::Animation::FastClip> fastClips(clips.size());
                    for(std::size_t i = 0; i < clips.size(); ++i)
                        fastClips[i] = Graphics::Animation::OptimizeClip(clips.at(i));

                    return std::make_shared<Graphics::Animation::Animation>(std::move(fastClips));
                }
                else
                {
                    return std::make_shared<Graphics::Animation::Animation>(std::move(clips));
                }
            }
            else
            {
                if(!err.empty()) logger.Error(err);
                return nullptr;
            }
        }

        template<>
        std::shared_ptr<Graphics::Model> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
        {
            std::string modelStr(reinterpret_cast<const char*>(bytes.data()), bytes.size());

            rapidjson::Document modelJson;
            rapidjson::ParseResult res = modelJson.Parse(modelStr.c_str()); 
            if(res.IsError())
            {
                std::string errorMsg = rapidjson::GetParseError_En(res.Code());
                Logger::Instance().Error("Failed to parse model (" + name + "): " + errorMsg + "(" + std::to_string(res.Offset()) + ")");
                return nullptr;
            }

            std::string meshName = modelJson["mesh"].GetString();
            std::string textureName = modelJson["texture"].GetString();

            auto textureId = Load<Graphics::Texture>(resourcePack.Read(textureName), textureName, {}, resourcePack);
            auto meshId = Load<Graphics::Mesh>(resourcePack.Read(meshName), meshName, {}, resourcePack);

            return std::make_shared<Graphics::Model>(meshId, textureId);
        }
    }
}
