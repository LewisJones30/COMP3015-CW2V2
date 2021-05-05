#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 MV;
uniform mat3 NormalMatrix;
uniform mat4 Proj;

void main()
{
	TexCoord = VertexTexCoord;
	Normal = normalize(NormalMatrix * VertexNormal);
	Position = vec3(MV * vec4(VertexPosition, 1.0));
	gl_Position = Proj * MV * vec4(VertexPosition, 1.0);
}