#version 460 core
#pragma shader_stage(fragment)

layout(binding=0) uniform sampler2D tex;

layout(location=0) in vec2 texCoords;
layout(location=1) in vec3 normal;

layout(location=0) out vec4 fragColor;

void main()
{
    fragColor = texture(tex, texCoords);
    //fragColor = vec4(0, 1, 0, 1);
}
