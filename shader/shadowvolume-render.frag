#version 460
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;



uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec4 LineColor;
in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout (location = 2) out vec4 FragColor;



const int levels = 3;
const float scaleFactor = 1.0 / levels;

uniform sampler2D Tex;

uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float Shininess;

layout (location = 0) out vec4 Ambient;
layout (location = 1) out vec4 DiffSpec;

void shade()
{
	vec3 s = normalize(vec3(LightPosition) - Position);
	vec3 v = normalize(vec3(-Position));
	vec3 r = reflect(-s, Normal);
	vec4 texColor = texture(Tex, TexCoord);
	Ambient = vec4(texColor.rgb * LightIntensity * Ka, 1.0);
	DiffSpec = vec4(texColor.rgb * LightIntensity * (Kd * max(dot(s, Normal), 0.0 ) + Ks * pow( max( dot(r,v), 0.0), Shininess)),1.0);
}

vec3 toonShade()
{
	vec3 s = normalize(LightPosition.xyz - GPosition.xyz);
	vec3 ambient = material.Ka;
	float cosine = dot(s, GNormal);
	vec3 diffuse = material.Kd * ceil(cosine * levels) * scaleFactor;

	return Light.Intensity * (ambient + diffuse);
}


void main()
{
	shade();
	if (GIsEdge == 1)
	{
		FragColor = LineColor;
	}
	else
	{
		FragColor = vec4(toonShade(), 1.0);
	}
}