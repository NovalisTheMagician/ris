#version 460 core
#pragma shader_stage(vertex)

#include "uiCommon.glsli"

layout(location=0) in vec2 attribPos;
layout(location=1) in vec2 attribTexCoords;

layout(location=2) out vec2 texCoords;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    vec2 pos = attribPos * perObject.size + perObject.position;
    pos = ((pos / perFrame.viewSize) * vec2(2)) - vec2(1);

    gl_Position = vec4(pos, 0, 1);
    texCoords = vec2(attribTexCoords.x, 1.0 - attribTexCoords.y);
}
