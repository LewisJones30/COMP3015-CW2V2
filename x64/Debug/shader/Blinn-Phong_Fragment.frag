#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Vec;
layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D tex1; //One raptor will have a single texture applied.
layout (binding = 1) uniform samplerCube SkyBoxTex; //Skybox.

uniform vec4 LightPosition;
uniform vec3 Kd;
uniform vec3 Ld;
uniform vec3 test1;
uniform struct LightInfo{
vec4 Position;
vec3 La;
vec3 L;
} lights[3];

uniform struct SpotLightInfo
{
vec3 Position;
vec3 L;
vec3 La;
vec3 Direction;
float Exponent;
float Cutoff;
} Spot[2];


uniform struct MaterialInfo{
vec3 Ka;
vec3 Kd;
vec3 Ks;
float Shininess;
} Material;

uniform struct FogInfo
{
float MaxDist;
float MinDist;
vec3 Color;
} Fog;

const int levels = 4;
const float scaleFactor = 1.0 / levels;

vec3 blinnphongSpotToon(vec3 position, vec3 normal) //T-Rex spotlight, includes toon lighting.
{
vec3 ambient = Spot[0].La * Material.Ka;
vec3 s = normalize(vec3(Spot[0].Position) - position);
float cosAng = dot(-s, normalize(Spot[0].Direction));
float angle = acos(cosAng);
float spotScale = 0.0;
vec3 diffuse = vec3(0.0);
vec3 spec = vec3(0.0);
if (angle < Spot[0].Cutoff)
{
    spotScale = pow(cosAng, Spot[0].Exponent);
    float sDotN = dot(s, normal);
    vec3 diffuse = Material.Kd * floor(sDotN * levels) * scaleFactor;

    if (sDotN > 0.0)
    {
    vec3 v = normalize(-position.xyz);
    vec3 h = normalize(v + s);
    spec = Material.Ks * pow(max(dot(h,normal),0.0), Material.Shininess);

    }
    return ambient + spotScale * Spot[0].L * (diffuse + spec);
    }
return ambient + spotScale * Spot[0].L * (diffuse + spec);
}

vec3 blinnphongSpot(vec3 position, vec3 normal) //Spotlight raptor, includes textures.
{
vec3 texColor = texture(tex1, TexCoord).rgb;
vec3 ambient = Material.Ka * texColor;
vec3 s = normalize(vec3(Spot[1].Position) - position);
float cosAng = dot(-s, normalize(Spot[1].Direction));
float angle = acos(cosAng);
float spotScale = 0.0;
vec3 diffuse = vec3(0.0);
vec3 spec = vec3(0.0);
if (angle < Spot[1].Cutoff)
{
    spotScale = pow(cosAng, Spot[1].Exponent);
    float sDotN = dot(s, normal);
    vec3 diffuse = Material.Kd * sDotN;

    if (sDotN > 0.0)
    {
    vec3 v = normalize(-position.xyz);
    vec3 h = normalize(v + s);
    spec = Material.Ks * pow(max(dot(h,normal),0.0), Material.Shininess);

    }
    return ambient + spotScale * Spot[1].L * (diffuse + spec);
    }
return ambient + spotScale * Spot[1].L * (diffuse + spec);
}




vec3 blinnphongSpotNoTexture(vec3 position, vec3 normal) //Spotlight raptor, includes textures.
{
vec3 ambient = Spot[1].La * Material.Ka;
vec3 s = normalize(vec3(Spot[1].Position) - position);
float cosAng = dot(-s, normalize(Spot[1].Direction));
float angle = acos(cosAng);
float spotScale = 0.0;
vec3 diffuse = vec3(0.0);
vec3 spec = vec3(0.0);
if (angle < Spot[1].Cutoff)
{
    spotScale = pow(cosAng, Spot[1].Exponent);
    float sDotN = dot(s, normal);
    vec3 diffuse = Material.Kd * sDotN;

    if (sDotN > 0.0)
    {
    vec3 v = normalize(-position.xyz);
    vec3 h = normalize(v + s);
    spec = Material.Ks * pow(max(dot(h,normal),0.0), Material.Shininess);

    }
    return ambient + spotScale * Spot[1].L * (diffuse + spec);
    }
return ambient + spotScale * Spot[1].L * (diffuse + spec);
}



//Used for the directional light.
vec3 blinnphongModel(vec3 position, vec3 normal)
{
vec3 ambient = lights[0].La;
vec3 s = normalize(vec3(lights[0].Position) - position);
float sDotN = dot(s,normal);
vec3 diffuse = Material.Kd * sDotN;
vec3 spec = vec3(0.0);
if (sDotN > 0.0)
{
vec3 v = normalize(-position.xyz);
vec3 h = normalize(v + s);
spec = Material.Ks * pow(max(dot(h,normal),0.0), Material.Shininess);
}
return ambient + lights[0].L * (diffuse + spec);
}

void main() { 
vec3 texColor = texture(SkyBoxTex, normalize(Vec)).rgb;
if (test1 == vec3(1.0f)) //Unused
{
    FragColor = vec4(blinnphongModel(Position, Normal), 1.0f);
}
else if (test1 == vec3(2.0f)) //Skybox
{
    FragColor = vec4(texColor, 1.0f);
}
else if (test1 == vec3(3.0f)) //T-Rex
{
    FragColor = vec4(blinnphongModel(Position, Normal), 1.0f) + vec4(blinnphongSpotToon(Position, normalize(Normal)), 1);
}
else if (test1 == vec3(4.0f)) //Floor plane
{
    FragColor = vec4(blinnphongModel(Position, Normal), 1.0f) + vec4(blinnphongSpotNoTexture(Position, normalize(Normal)), 1) + vec4(blinnphongSpotToon(Position, normalize(Normal)), 1);
}
else if (test1 == vec3(5.0f))//Raptor 1 uses one texture, no fog.
{
    FragColor = vec4(blinnphongModel(Position, Normal), 1.0f) + vec4(blinnphongSpotToon(Position, normalize(Normal)), 1) + vec4(blinnphongSpot(Position, normalize(Normal)), 1);
}
else //Raptor 2 uses fog.
{
    float dist = abs(Position.z);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );
    vec4 shadeColor = vec4(blinnphongModel(Position, Normal), 1.0f);
    vec3 color = mix(Fog.Color.rgb, shadeColor.rgb, fogFactor);
    FragColor = vec4(color, 1.0) + vec4(blinnphongSpotToon(Position, normalize(Normal)), 1) + vec4(blinnphongSpot(Position, normalize(Normal)), 1);
}
    
}