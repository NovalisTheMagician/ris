#version 450
#pragma shader_stage(fragment)

#include "uiCommon.glsli"

layout(binding=0) uniform sampler2D tex;

layout(location=2) in vec2 texCoords;

layout(location=0) out vec4 fragColor;

void main()
{
    float sdfSample = texture(tex, texCoords).r;
    float delta = 0.1;
    float alpha = smoothstep(0.5 - delta, 0.5 + delta, sdfSample);
    fragColor = vec4(perObject.color.rgb, alpha);
    //fragColor = vec4(0, 1, 0, 1);
}
