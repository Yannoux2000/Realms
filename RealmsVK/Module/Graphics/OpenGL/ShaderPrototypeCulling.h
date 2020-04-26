#pragma once
#include "Shader.h"
#include "GameShaderProgram.h"

class ShaderPrototype1 {
public:
	static inline Shader vertexShader () {
		const GLchar* __vertexShaderSrc = R"glsl(
	#version 150 core
	in ivec3 pos;
	in int cull;
	in int col;

	out int vColor;
	out int vCulling;

	uniform int bfculling;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

	void main () {
		vColor = col;
		vCulling = cull;
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
	in int vCulling[];
	out int gColor;

	uniform int bfculling;
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

		int cullflags = vCulling[0] & bfculling;


		if(cullflags & 0x80){ //if the voxel is hidden

			//draw nothing

		} else {
			if(cullflags & 0x01){ //Xp
				AddQuad(center + dx, dy, dz);
			}

			if(cullflags & 0x02){ //Xn
				AddQuad(center - dx, dz, dy);
			}

			if(cullflags & 0x04){ //Yp
				AddQuad(center + dy, dz, dx);
			}

			if(cullflags & 0x08){ //Yn
				AddQuad(center - dy, dx, dz);
			}

			if(cullflags & 0x10){ //Zp
				AddQuad(center + dz, dx, dy);
			}

			if(cullflags & 0x20){ //Zn
				AddQuad(center - dz, dy, dx);
			}
		}

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
		return ShaderPrototype1::vertexShader ();
	}

	static inline Shader geometryShader () {
		const GLchar* __geometryShaderSrc = R"glsl(
	#version 150 core
	layout(points) in;
	layout(line_strip, max_vertices = 36) out;

	in int vColor[];
	in int vCulling[];
	out int gColor;

	uniform int bfculling;
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
		gl_Position = center + (dx - dy);
		EmitVertex();
		EndPrimitive();
	}

	void main()
	{
		gColor = vColor[0];
		vec4 center = gl_in[0].gl_Position;
		mat4 mvp = (proj * view * model);

		vec4 dx = mvp[0] / 2.5f;
		vec4 dy = mvp[1] / 2.5f;
		vec4 dz = mvp[2] / 2.5f;

		int cullflags = vCulling[0] & bfculling;


		if(cullflags & 0x80){ //if the voxel is hidden

			//draw nothing

		} else {
			if(cullflags & 0x01){ //Xp
				AddQuad(center + dx, dy, dz);
			}

			if(cullflags & 0x02){ //Xn
				AddQuad(center - dx, dz, dy);
			}

			if(cullflags & 0x04){ //Yp
				AddQuad(center + dy, dz, dx);
			}

			if(cullflags & 0x08){ //Yn
				AddQuad(center - dy, dx, dz);
			}

			if(cullflags & 0x10){ //Zp
				AddQuad(center + dz, dx, dy);
			}

			if(cullflags & 0x20){ //Zn
				AddQuad(center - dz, dy, dx);
			}
		}
	})glsl";

		return Shader (GL_GEOMETRY_SHADER, __geometryShaderSrc);
	}

	static inline Shader fragmentShader () {
		return ShaderPrototype1::fragmentShader ();
	}
};