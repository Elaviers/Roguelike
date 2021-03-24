#version 410
uniform uvec4 uColour;
uniform sampler2D T_Diffuse;

layout(location = 0) in vec2 UV;

layout(location = 0) out uvec2 colour;

void main()
{
	vec4 tex = texture(T_Diffuse, UV);
		if (tex.a < 0.01) discard;
	
	colour = uColour.rg;
}
