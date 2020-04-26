#pragma once
#include "Shader.h"
#include "GameShaderProgram.h"

class ShaderPrototype1 {
public:
	static inline Shader vertexShader () {
		const GLchar* __vertexShaderSrc = R"glsl(
	#version 150 core
	in ivec3 pos;
	in int col;

	out int vColor;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

	void main () {
		vColor = col;
		gl_Position = proj * view * model * vec4 (pos, 1.0);
	})glsl";

		return Shader (GL_VERTEX_SHADER, __vertexShaderSrc);
	}

	static inline Shader geometryShader () {
		const GLchar* __geometryShaderSrc = R"glsl(
	#version 150 core
	layout(points) in;
	layout(triangle_strip, max_vertices = 24) out;

	in int vColor[];
	out int gColor;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

	void AddQuad(vec4 center, vec4 dy, vec4 dx) {

		gl_Position = center + (dx - dy);
		EmitVertex();
		gl_Position = center + (-dx - dy);
		EmitVertex();
		gl_Position = center + (dx + dy);
		EmitVertex();
		gl_Position = center + (-dx + dy);
		EmitVertex();
		EndPrimitive();
	}

	void main()
	{
		gColor = vColor[0];
		vec4 center = gl_in[0].gl_Position;

		mat4 mvp = (proj * view * model);

		vec4 dx = mvp[0] / 2.0f;
		vec4 dy = mvp[1] / 2.0f;
		vec4 dz = mvp[2] / 2.0f;

		AddQuad(center + dx, dy, dz);
		AddQuad(center - dx, dz, dy);
		AddQuad(center + dy, dz, dx);
		AddQuad(center - dy, dx, dz);
		AddQuad(center + dz, dx, dy);
		AddQuad(center - dz, dy, dx);
	})glsl";

		return Shader (GL_GEOMETRY_SHADER, __geometryShaderSrc);
	}

	static inline Shader fragmentShader () {
		const GLchar* __fragmentShaderSrc = R"glsl(
	#version 150 core

	in flat int gColor;

	out vec4 outColor;

	uniform vec4 palette[256];

	void main () {
		outColor = palette[gColor];
	})glsl";

		return Shader (GL_FRAGMENT_SHADER, __fragmentShaderSrc);
	}
};

class ShaderPrototypeWired1 {
public:
	static inline Shader vertexShader () {
		const GLchar* __vertexShaderSrc = R"glsl(
	#version 150 core
	in ivec3 pos;
	in int col;

	out int vColor;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

	void main () {
		vColor = col;
		gl_Position = proj * view * model * vec4 (pos, 1.0);
	})glsl";

		return Shader (GL_VERTEX_SHADER, __vertexShaderSrc);
	}

	static inline Shader geometryShader () {
		const GLchar* __geometryShaderSrc = R"glsl(
	#version 150 core
	layout(points) in;
	layout(line_strip, max_vertices = 24) out;

	in int vColor[];
	out int gColor;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

	void AddQuad(vec4 center, vec4 dy, vec4 dx) {

		gl_Position = center + (dx - dy);
		EmitVertex();
		gl_Position = center + (-dx - dy);
		EmitVertex();
		gl_Position = center + (-dx + dy);
		EmitVertex();
		gl_Position = center + (dx + dy);
		EmitVertex();
		EndPrimitive();
	}

	void main()
	{
		gColor = vColor[0];
		vec4 center = gl_in[0].gl_Position;

		mat4 mvp = (proj * view * model);

		vec4 dx = mvp[0] / 1.99f;
		vec4 dy = mvp[1] / 1.99f;
		vec4 dz = mvp[2] / 1.99f;

		AddQuad(center + dx, dy, dz);
		AddQuad(center - dx, dz, dy);
		AddQuad(center + dy, dz, dx);
		AddQuad(center - dy, dx, dz);
		AddQuad(center + dz, dx, dy);
		AddQuad(center - dz, dy, dx);
	})glsl";

		return Shader (GL_GEOMETRY_SHADER, __geometryShaderSrc);
	}

	static inline Shader fragmentShader () {
		const GLchar* __fragmentShaderSrc = R"glsl(
	#version 150 core

	in flat int gColor;

	out vec4 outColor;

	uniform vec4 palette[256];

	void main () {
		outColor = palette[gColor];
	})glsl";

		return Shader (GL_FRAGMENT_SHADER, __fragmentShaderSrc);
	}
};