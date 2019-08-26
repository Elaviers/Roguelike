#version 410
uniform vec4 Colour;
uniform sampler2D T_Diffuse;
uniform bool IsFont;

layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 VertexColour;

out vec4 OutColour;

void main()
{
	if (IsFont)
	{
		float alpha = texture(T_Diffuse, vec2(UV.x, 1 - UV.y)).r;

		if (alpha > 0)
		{
			OutColour = Colour * VertexColour;
			OutColour.a *= alpha;
		}
		else
			OutColour = vec4(0);
	}
	else
		OutColour = Colour * VertexColour * texture(T_Diffuse, UV);
}
