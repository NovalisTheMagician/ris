#version 460 core
#pragma shader_stage(vertex)

#include "model.glsli"

layout(location=0) in vec3 attribPos;
layout(location=1) in vec3 attribNormal;
layout(location=2) in vec2 attribTexCoords;
layout(location=3) in ivec4 attribJoints;
layout(location=4) in vec4 attribWeights;

layout(location=0) out vec2 texCoords;
layout(location=1) out vec3 normal;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = perFrame.viewProjection * perObject.world * vec4(attribPos, 1.0);

    texCoords = attribTexCoords;
    normal = mat3(perObject.world) * attribNormal;
}
