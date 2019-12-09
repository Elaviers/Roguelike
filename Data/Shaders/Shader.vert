#version 410
#define BONE_COUNT 25
uniform mat4 M_Model;
uniform mat4 M_View;
uniform mat4 M_Projection;
uniform mat4 Bones[BONE_COUNT];
uniform vec2 UVOffset;
uniform vec2 UVScale = vec2(1);

layout(location = 0) in vec3 WorldPosition_IN;
layout(location = 1) in vec2 UV_IN;
layout(location = 2) in vec4 VertexColour_IN;
layout(location = 3) in vec3 ModelTangent_IN;
layout(location = 4) in vec3 ModelBitangent_IN;
layout(location = 5) in vec3 ModelNormal_IN;

#define BONES_PER_VERTEX 4
layout(location = 6) in vec4 BoneIndices;
layout(location = 7) in vec4 BoneWeights;

layout(location = 0) out vec3 WorldPosition;
layout(location = 1) out vec2 UV;
layout(location = 2) out vec4 VertexColour;
layout(location = 3) out mat3 TBN;

void main()
{
	VertexColour = vec4(0.f, 0.f, 0.f, 1.f);

	vec3 position = vec3(0);
	vec3 normal = vec3(0);
	vec3 tangent = vec3(0);
	vec3 bitangent = vec3(0);
	float TotalWeight = 0.f;
	for (int i = 0; i < BONES_PER_VERTEX && BoneWeights[i] != 0.f; ++i)
	{
		TotalWeight += BoneWeights[i];
		
		mat4 transform = Bones[int(BoneIndices[i])];
		float weight = BoneWeights[i];

		position +=		(transform * vec4(WorldPosition_IN, 1)).xyz		* weight;
		normal +=		(transform * vec4(ModelNormal_IN, 0)).xyz		* weight;
		tangent +=		(transform * vec4(ModelTangent_IN, 0)).xyz		* weight;
		bitangent +=	(transform * vec4(ModelBitangent_IN, 0)).xyz	* weight;
	}

	if (TotalWeight == 0.f)
	{
		position = WorldPosition_IN;
		normal = ModelNormal_IN;
		tangent = ModelTangent_IN;
		bitangent = ModelBitangent_IN;
	}

	WorldPosition = (M_Model * vec4(position, 1)).xyz;
	gl_Position = M_Projection * M_View * vec4(WorldPosition, 1);

	VertexColour = VertexColour_IN;
	UV = vec2(UV_IN.x, 1 - UV_IN.y) * UVScale + UVOffset;
	TBN = 
		mat3(
			normalize((M_Model * vec4(tangent, 0)).xyz), 
			normalize((M_Model * vec4(bitangent, 0)).xyz), 
			normalize((M_Model * vec4(normal, 0)).xyz)
			);
}
