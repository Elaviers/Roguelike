#version 410
uniform mat4 M_Model;
uniform mat4 M_View;
uniform mat4 M_Projection;

layout(location = 0) in vec3 WorldPosition_IN;
layout(location = 1) in vec2 UV_IN;
layout(location = 2) in vec4 VertexColour_IN;
layout(location = 3) in vec3 ModelTangent_IN;
layout(location = 4) in vec3 ModelBitangent_IN;
layout(location = 5) in vec3 ModelNormal_IN;

layout(location = 0) out vec3 WorldPosition;
layout(location = 1) out vec2 UV;
layout(location = 2) out vec4 VertexColour;
layout(location = 3) out mat3 TBN;

void main()
{
	WorldPosition = (M_Model * vec4(WorldPosition_IN, 1)).xyz;
	gl_Position = M_Projection * M_View * vec4(WorldPosition, 1);

	UV = UV_IN;
	VertexColour = VertexColour_IN;
	TBN = mat3(vec3(M_Model * vec4(ModelTangent_IN, 0)), vec3(M_Model * vec4(ModelBitangent_IN, 0)), vec3(M_Model * vec4(ModelNormal_IN, 0)));
}
