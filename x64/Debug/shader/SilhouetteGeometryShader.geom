#version 460

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 15) out;

out vec3 GNormal;
out vec3 GPosition;

flat out int GIsEdge;

in vec3 VNormal[];
in vec3 VPosition[];

uniform float EdgeWidth;
uniform float PctExtend;

bool isFrontFacing(vec3 a, vec3 b, vec3 c)
{
	return ((a.x * b.y - b.x * a.y) + (b.x * c.y - c.x * b.y) + (c.x * a.y - a.x * c.y)) > 0;
}

void emitEdgeQuad(vec3 a, vec3 b)
{
	
	vec2 ext = PctExtend * (b.xy - a.xy);
	vec2 v = normalize(b.xy - a.xy);
	vec2 n = vec2(-v.y, v.x) * EdgeWidth;

	GIsEdge = 1;
	
	gl_Position = vec4(a.xy - ext, a.z, 1.0); EmitVertex();
	gl_Position = vec4(a.xy - n - ext, a.z, 1.0); EmitVertex();
	gl_Position = vec4(b.xy + ext, b.z, 1.0); EmitVertex();
	gl_Position = vec4(b.xy - n + ext, b.z, 1.0); EmitVertex();

	EndPrimitive();
}


void main()
{
	vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
	vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
	vec3 p3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
	vec3 p4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
	vec3 p5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;
	if (isFrontFacing(p0, p2, p4)) {
	if (! isFrontFacing(p0, p1, p2)) emitEdgeQuad(p0, p2);
	if (! isFrontFacing(p2, p3, p4)) emitEdgeQuad(p2, p4);
	if (! isFrontFacing(p4, p5, p0)) emitEdgeQuad(p4, p0);
	}
	GIsEdge = 0;

	GNormal = VNormal[0];
	GPosition = VPosition[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	GNormal = VNormal[2];
	GPosition = VPosition[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	GNormal = VNormal[4];
	GPosition = VPosition[4];
	gl_Position = gl_in[4].gl_Position;
	EmitVertex();

	EndPrimitive();
}