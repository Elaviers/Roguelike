#version 410
uniform vec4 Colour;

layout(location = 2) in vec4 VertexColour;

out vec4 OutColour;

void main()
{
	if (VertexColour == 0)
		OutColour = Colour;
	else if (Colour == 0)
		OutColour = VertexColour;
	else
		OutColour = Colour * VertexColour;
}
