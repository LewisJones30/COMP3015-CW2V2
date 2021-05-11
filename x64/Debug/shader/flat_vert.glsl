#version 460
layout (location = 0) in vec3 VertexPosition;

uniform mat4 Proj;
uniform mat4 MV;

void main()
{
	gl_Position = Proj * MV * vec4(VertexPosition, 1.0);
}