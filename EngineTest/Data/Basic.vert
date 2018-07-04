#version 410
uniform mat4 M_Model;
uniform mat4 M_View;
uniform mat4 M_Projection;

in vec3 WorldPosition;

void main()
{
	gl_Position = M_Projection * M_View * M_Model * vec4(WorldPosition, 1);
}
