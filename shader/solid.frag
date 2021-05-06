#version 460
in vec3 Position;
in vec3 Normal;

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout (location = 0) out vec4 FragColor;


const int levels = 3;
const float scaleFactor = 1.0 / levels;


uniform sampler2D DiffSpecTex;


void main()
{
	vec4 diffSpec = texelFetch(DiffSpecTex, ivec2(gl_FragCoord), 0);
	FragColor = vec4(diffSpec.xyz, 1);
}
