#include "loader/ModelLoader.hpp"

#include "loader/TextureLoader.hpp"

#include "graphics/Mesh.hpp"
#include "graphics/Model.hpp"
#include "graphics/Animation.hpp"

#include "misc/Logger.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <map>
#include <vector>
#include <array>

#include <fmt/format.h>

namespace RIS::Loader
{
    namespace GLTFHelper
    {
        template<typename T, size_t S = sizeof(T)>
        static std::array<unsigned char, S> GetComponents(const T &comp)
        {
            std::array<unsigned char, S> d;
            std::memcpy(d.data(), &comp, S);
            return d;
        }

        static std::vector<unsigned char> Interleave(const std::vector<glm::vec3> &positions, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &texCoords, const std::vector<glm::i16vec4> &joints, const std::vector<glm::vec4> &weights)
        {
            std::vector<unsigned char> data;
            size_t size = positions.size();
            for(size_t i = 0; i < size; ++i)
            {
                auto p = GetComponents(positions.at(i));
                data.insert(data.end(), p.cbegin(), p.cend());
                auto n = GetComponents(normals.at(i));
                data.insert(data.end(), n.cbegin(), n.cend());
                auto t = GetComponents(texCoords.at(i));
                data.insert(data.end(), t.cbegin(), t.cend());
                auto j = GetComponents(joints.at(i));
                data.insert(data.end(), j.cbegin(), j.cend());
                auto w = GetComponents(weights.at(i));
                data.insert(data.end(), w.cbegin(), w.cend());
            }
            return data;
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

        template<typename T, std::size_t Size = sizeof(T)>
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
        }

        template<typename T, std::size_t Size = sizeof(T)>
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
            Graphics::Animation::Interpolation interpolation = Graphics::Animation::Interpolation::CONSTANT;
            if(sampler.interpolation == "LINEAR")
                interpolation = Graphics::Animation::Interpolation::LINEAR;
            else if(sampler.interpolation == "CUBICSPLINE")
                interpolation = Graphics::Animation::Interpolation::CUBIC;

            bool isSamplerCubic = interpolation == Graphics::Animation::Interpolation::CUBIC;
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

        static Graphics::Animation::Pose LoadRestPose(const tinygltf::Model &model, const tinygltf::Skin &skin, std::vector<std::string> &jointNames, Logger &logger)
        {
            auto nodeToTransform = [](const tinygltf::Node &node)
            {
                Graphics::Transform transform;
                if(node.matrix.size() > 0)
                {
                    glm::mat4 matrix = glm::make_mat4(node.matrix.data());
                    transform = Graphics::Mat4ToTransform(matrix);
                }
                if(node.translation.size() > 0)
                    transform.position = glm::make_vec3(node.translation.data());
                if(node.scale.size() > 0)
                    transform.scale = glm::make_vec3(node.scale.data());
                if(node.rotation.size() > 0)
                    transform.rotation = glm::make_quat(node.rotation.data());
                return transform;
            };

            const auto &nodeBoneMap = CreateNodeBoneMap(model, skin);

            std::size_t boneCount = skin.joints.size();
            Graphics::Animation::Pose pose(boneCount);
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
                Graphics::Transform t = nodeToTransform(node);
                pose.SetLocalTransform(boneId, t);
                int parent = getParent(i);

                int parentBoneId = parent == -1 ? -1 : static_cast<int>(nodeBoneMap.at(parent));
                pose.SetParent(boneId, parentBoneId);
                
                names[boneId] = node.name;
            }

            jointNames = names;
            return pose;
        }

        static Graphics::Animation::Pose LoadBindPose(const tinygltf::Model &model, const tinygltf::Skin &skin, RIS::Graphics::Animation::Pose &restPose, Logger &logger)
        {
            std::size_t numBones = restPose.Size();
            std::vector<Graphics::Transform> worldBindPose(numBones);
            for(std::size_t i = 0; i < numBones; ++i)
                worldBindPose[i] = restPose.GetGlobalTransform(i);

            const tinygltf::Accessor &matAccessor = model.accessors.at(skin.inverseBindMatrices);

            std::vector<float> invBindAccessor;
            auto compCount = GetCompCount(matAccessor);

            if(compCount != 16) throw std::runtime_error("compCount mismatch");

            GetScalarValues(model, invBindAccessor, compCount, matAccessor);

            std::size_t numJoints = skin.joints.size();
            for(std::size_t i = 0; i < numJoints; ++i)
            {
                float *matrix = invBindAccessor.data() + i * compCount;
                glm::mat4 invBindMatrix = glm::make_mat4(matrix);
                glm::mat4 bindMatrix = glm::inverse(invBindMatrix);
                Graphics::Transform bindTransform = Graphics::Mat4ToTransform(bindMatrix);
                worldBindPose[i] = bindTransform;
            }

            Graphics::Animation::Pose bindPose(restPose);
            for(std::size_t i = 0; i < numBones; ++i)
            {
                Graphics::Transform current = worldBindPose.at(i);
                int parentId = bindPose.GetParent(i);
                if(parentId >= 0)
                {
                    Graphics::Transform parent = worldBindPose.at(parentId);
                    current = Combine(Inverse(parent), current);
                }
                bindPose.SetLocalTransform(i, current);
            }

            return bindPose;
        }
    }

    template<>
    std::shared_ptr<Graphics::Mesh> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        auto &logger = Logger::Instance();

        tinygltf::TinyGLTF gltfLoader;
        tinygltf::Model model;
        std::string err, warn;

        bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(bytes.data()), static_cast<unsigned int>(bytes.size()));
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

            const tinygltf::Mesh &mesh = model.meshes.at(0);
            const tinygltf::Primitive &primitive = mesh.primitives.at(0);

            if(!checkAttribs(primitive.attributes))
                return nullptr;

            std::size_t numElements = model.accessors.at(primitive.attributes.at("POSITION")).count;

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

            std::vector<unsigned char> vertices = GLTFHelper::Interleave(positions, normals, texCoords, joints, weights);

            std::vector<unsigned short> indices;
            const tinygltf::Accessor &indexAccessor = model.accessors.at(primitive.indices);
            GLTFHelper::GetValues(model, indexAccessor, indices);

            Graphics::VertexBuffer vertexBuffer(vertices);
            Graphics::IndexBuffer indexBuffer(indices);
            
            int numIndices = static_cast<int>(model.accessors.at(primitive.indices).count);

            return std::make_shared<Graphics::Mesh>(std::move(vertexBuffer), std::move(indexBuffer), numIndices);
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

        bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(bytes.data()), static_cast<unsigned int>(bytes.size()));
        if(!warn.empty()) logger.Warning(warn);
        if(result)
        {
            if(model.skins.size() == 0)
            {
                logger.Warning(fmt::format("({}): no skin found", name));
                return nullptr;
            }

            if(model.skins.size() > 1)
            {
                logger.Warning(fmt::format("({}): more than 1 skin found. using the first one", name));
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

        bool result = gltfLoader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(bytes.data()), static_cast<unsigned int>(bytes.size()));
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

                for(const auto &channel : animation.channels)
                {
                    //const tinygltf::Node &targetNode = model.nodes.at(channel.target_node);
                    int nodeId = static_cast<int>(nodeBoneMap.at(channel.target_node));
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
            Logger::Instance().Error(fmt::format("Failed to parse model ({}): {}({})", name, errorMsg, std::to_string(res.Offset())));
            return nullptr;
        }

        std::string meshName = modelJson["mesh"].GetString();
        std::string textureName = modelJson["texture"].GetString();

        auto textureId = Load<Graphics::Texture>(resourcePack.Read(textureName), textureName, {}, resourcePack);
        auto meshId = Load<Graphics::Mesh>(resourcePack.Read(meshName), meshName, {}, resourcePack);

        if(!textureId || !meshId)
            return nullptr;

        return std::make_shared<Graphics::Model>(meshId, textureId);
    }
}
