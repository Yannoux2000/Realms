#version 450

layout (location = 0) out vec2 outUV;

out gl_PerVertex 
{
	vec4 gl_Position;
};

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
	//outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	//gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);
	
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
