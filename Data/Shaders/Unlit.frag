#version 410
uniform vec4 Colour;
uniform sampler2D T_Diffuse;

layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 VertexColour;

out vec4 OutColour;

void main()
{
	OutColour = Colour * VertexColour * texture(T_Diffuse, UV);
}
