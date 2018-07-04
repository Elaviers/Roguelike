#version 410
uniform mat4 M_View;
uniform mat4 M_Projection;

layout(location = 0) in vec3 Position;

out vec3 UVW;

void main()
{
	UVW = Position;
	gl_Position = (M_Projection * M_View * vec4(Position, 1)).xyww;
}
