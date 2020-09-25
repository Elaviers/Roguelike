#version 410
uniform mat4 M_Model;
uniform mat4 M_View;
uniform mat4 M_Projection;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV_IN;

layout(location = 0) out vec2 UV;

void main()
{
	UV = UV_IN;
	gl_Position = M_Projection * M_View * M_Model * vec4(Position, 1);
}
