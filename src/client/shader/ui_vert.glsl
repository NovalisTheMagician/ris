#version 450 core
#pragma shader_stage(vertex)

#include "common2d.glsli"

layout(location=0) in vec2 attribPos;
layout(location=1) in vec2 attribTexCoords;

layout(location=2) out vec2 texCoords;

void main()
{
    gl_Position = perFrame.projection * perFrame.view * perObject.world * vec4(attribPos, 0, 1);
    texCoords = attribTexCoords;
}
