#version 460 core

layout(location=0) in vec2 pos;

out gl_PerVertex 
{
   vec4 gl_Position;
};

void main() 
{
   gl_Position = vec4(pos, 0, 1);
}