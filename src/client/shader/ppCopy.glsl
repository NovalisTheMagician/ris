#version 460 core
#pragma shader_stage(fragment)

//#include "ppCommon.glsli"

layout(binding=0) uniform sampler2D tex;

layout(location=2) in vec2 texCoords;

layout(location=0) out vec4 fragColor;

void main()
{
    fragColor = texture(tex, texCoords);
}
