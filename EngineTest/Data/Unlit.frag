#version 410
uniform vec4 Colour;

layout(location = 2) in vec4 VertexColour;

out vec4 OutColour;

void main()
{
	OutColour = VertexColour * Colour;
}
