#version 450
#pragma shader_stage(fragment)

#include "uiCommon.glsli"

layout(binding=0) uniform sampler2D tex;

layout(location=2) in vec2 texCoords;

layout(location=0) out vec4 fragColor;

void main()
{
    vec4 texColor = texture(tex, texCoords);
    fragColor = texColor * perObject.color;
    //fragColor = perObject.color;
}
