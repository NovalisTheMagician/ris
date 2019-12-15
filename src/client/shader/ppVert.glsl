#version 460 core
#pragma shader_stage(vertex)

#include "ppCommon.glsli"

layout(location=0) in vec2 attribPosition;
layout(location=1) in vec2 attribTexCoord;

layout(location=2) out vec2 texCoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = vec4(attribPosition, 0, 1);
    texCoord = attribTexCoord;
}
