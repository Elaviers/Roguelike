#version 410
uniform samplerCube Skybox;

in vec3 UVW;

out vec4 OutColour;

void main()
{
	OutColour = texture(Skybox, UVW);
}
