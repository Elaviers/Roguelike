#version 410
uniform vec4 Colour;

uniform bool Blend;
uniform vec4 BlendFrom;
uniform vec4 BlendTo;

uniform sampler2D T_Diffuse;
uniform bool IsFont;

layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 VertexColour;

out vec4 OutColour;

void main()
{
	vec4 tex = texture(T_Diffuse, UV);

	if (Blend) 
		OutColour = mix(BlendFrom, BlendTo, tex.r) * vec4(1, 1, 1, tex.a);
	else
	{
		if (IsFont)
		{
			float alpha = tex.r;

			if (alpha > 0)
			{
				OutColour = Colour * VertexColour;
				OutColour.a *= alpha;
			}
			else
				OutColour = vec4(0);
		}
		else
			OutColour = Colour * VertexColour * tex;
	}
}
