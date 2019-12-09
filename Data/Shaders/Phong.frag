#version 410
#define LIGHT_COUNT 8
#define BLINN false
#define FLAT false

struct Light
{
	vec3 Position;
	vec3 Colour;
	float Radius;
};

uniform Light Lights[LIGHT_COUNT];
uniform mat4 M_Model;
uniform mat4 M_View;
uniform samplerCube Cubemap;
uniform sampler2D T_Diffuse;
uniform sampler2D T_Normal;
uniform sampler2D T_Specular;
uniform sampler2D T_Reflection;
uniform vec4 Colour;

layout(location = 0) in vec3 WorldPosition;
layout(location = 1) in vec2 UV;
layout(location = 3) in mat3 TBN;

out vec4 OutColour;

vec3 AmbientLight = vec3(0.1);
float shine = 32;

void main()
{
	vec3 UVNormal = texture(T_Normal, UV).rgb * 2 - 1;

	vec3 WorldNormal;
	if (FLAT)	WorldNormal = normalize(cross(dFdy(WorldPosition), dFdx(WorldPosition)));
	else		WorldNormal = normalize(TBN * UVNormal).xyz;

	mat4 CameraTransform = inverse(M_View);
	vec3 CameraPosition = vec3(CameraTransform[3][0], CameraTransform[3][1], CameraTransform[3][2]);
	
	float SurfaceSpecular  = texture(T_Specular, UV).r;
	float SurfaceReflectivity = texture(T_Reflection, UV).r;
	vec3 SurfaceToCamera = normalize(CameraPosition - WorldPosition);
	vec3 SurfaceColour = (1.0 - SurfaceReflectivity) * texture(T_Diffuse, UV).rgb;

	float Distance;
	vec3 Diffuse;
	vec3 Specular;
	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		if (Lights[i].Radius != 0.0)
		{
			float Distance = length(Lights[i].Position - WorldPosition);
			if (Lights[i].Radius > 0.0 && Distance > Lights[i].Radius) continue;

			vec3 SurfaceToLight = normalize(Lights[i].Position - WorldPosition);
			float DiffuseAmount = clamp(dot(WorldNormal, SurfaceToLight), 0.0, 1.0);
	
			if (DiffuseAmount > 0.0)
			{
				float Attenuation;
				if (Lights[i].Radius < 0.0)	Attenuation = 1.0;
				else						Attenuation = 1.0 - ((Distance * Distance) / (Lights[i].Radius * Lights[i].Radius));

				float SpecularAmount;
				if (BLINN)
				{
					vec3 Halfway = normalize(SurfaceToLight + SurfaceToCamera);
					SpecularAmount = max(0.0, dot(Halfway, WorldNormal));
				}
				else
				{
					vec3 ReflectionDirection = reflect(-SurfaceToLight, WorldNormal);
					SpecularAmount = max(0.0, dot(ReflectionDirection, SurfaceToCamera));
				}

				Diffuse += Attenuation * DiffuseAmount * Lights[i].Colour * SurfaceColour;
				Specular += Attenuation * pow(SpecularAmount, shine) * Lights[i].Colour * SurfaceSpecular;
			}
		}
	}

	vec3 CubemapReflection = reflect(-SurfaceToCamera, WorldNormal);
	vec3 Reflection = SurfaceReflectivity * texture(Cubemap, CubemapReflection).rgb;

	OutColour = Colour * vec4(SurfaceColour * AmbientLight + Diffuse + Reflection + Specular, 1.0);
}
