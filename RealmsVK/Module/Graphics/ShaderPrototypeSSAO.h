#pragma once
#include "Shader.h"
#include "GameShaderProgram.h"

class ShaderPrototype1 {
public:
	static inline Shader vertexShader () {
		const GLchar* __vertexShaderSrc = R"glsl(
	#version 330 core

	in ivec3 aPos;
	in int aCulling;
	in int aCol;

	out vec3 vPos;
	out int vCulling;
	out int vColor;
	
	uniform int bfculling;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main () {
		vColor = aCol;
		vCulling = aCulling;
		vPos = vec3(model * vec4(aPos, 1.0));
		gl_Position = projection * view * model * vec4 (aPos, 1.0);
	})glsl";

		return Shader (GL_VERTEX_SHADER, __vertexShaderSrc);
	}

	static inline Shader geometryShader () {
		const GLchar* __geometryShaderSrc = R"glsl(
	#version 330 core
	layout(points) in;
	layout(triangle_strip, max_vertices = 24) out;

	in int vColor[];
	in int vCulling[];
	in vec3 vPos[];
	
	out int gColor;
	out vec3 gNormal;
	out vec3 gPos;

	uniform int bfculling;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void AddQuad(vec4 center, vec4 dy, vec4 dx) {

		gPos = vec3(center + (dx - dy));
		gl_Position = center + (dx - dy);
		EmitVertex();

		gPos = vec3(center + (-dx - dy));
		gl_Position = center + (-dx - dy);
		EmitVertex();

		gPos = vec3(center + (dx + dy));
		gl_Position = center + (dx + dy);
		EmitVertex();

		gPos = vec3(center + (-dx + dy));
		gl_Position = center + (-dx + dy);
		EmitVertex();

		EndPrimitive();
	}

	void main() {
		vec4 center = gl_in[0].gl_Position;
		mat4 mvp = (projection * view * model);

		vec4 dx = mvp[0] / 1.9f;
		vec4 dy = mvp[1] / 1.9f;
		vec4 dz = mvp[2] / 1.9f;

		int cullflags = vCulling[0] & bfculling;
		if(cullflags & 0x80){ //if the voxel is hidden

			//draw nothing

		} else {
			if(cullflags & 0x01){ //Xp
				gColor = vColor[0];
				gNormal = vec3(normalize(dx));
				AddQuad(center + dx, dy, dz);
			}

			if(cullflags & 0x02){ //Xn
				gColor = vColor[0];
				gNormal = vec3(normalize(-dx));
				AddQuad(center - dx, dz, dy);
			}

			if(cullflags & 0x04){ //Yp
				gColor = vColor[0];
				gNormal = vec3(normalize(dy));
				AddQuad(center + dy, dz, dx);
			}

			if(cullflags & 0x08){ //Yn
				gColor = vColor[0];
				gNormal = vec3(normalize(-dy));
				AddQuad(center - dy, dx, dz);
			}

			if(cullflags & 0x10){ //Zp
				gColor = vColor[0];
				gNormal = vec3(normalize(dz));
				AddQuad(center + dz, dx, dy);
			}

			if(cullflags & 0x20){ //Zn
				gColor = vColor[0];
				gNormal = vec3(normalize(-dz));
				AddQuad(center - dz, dy, dx);
			}
		}

	})glsl";

		return Shader (GL_GEOMETRY_SHADER, __geometryShaderSrc);
	}

	static inline Shader fragmentShader () {
		const GLchar* __fragmentShaderSrc = R"glsl(
	#version 330 core

	in vec3 gPos;
	in vec3 gNormal;
	in flat int gColor;

	out vec4 outColor;

	struct DirLight {
		vec3 direction;

	    vec3 ambient;
	    vec3 diffuse;
	    vec3 specular;
	};

	uniform DirLight dirLight;

	uniform int bfculling;

	uniform vec3 viewPos;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	uniform vec4 palette[256];

	vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 color);

	void main () {
	    vec3 norm = vec3(normalize(gNormal));
	    vec3 viewDir = normalize(viewPos - gPos);
		vec3 result = CalcDirLight(dirLight, norm, viewDir, palette[gColor]);

		outColor = vec4(result, 1.0);
	}

	
	vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 color) {
		vec3 lightDir = normalize(-light.direction);

		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);

		// specular shading
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
		// combine results
		vec3 ambient  = light.ambient;
		vec3 diffuse  = light.diffuse  * diff;
		vec3 specular = light.specular * spec * 0.25;
		return (ambient + diffuse + specular) * vec3(color);
	}
	)glsl";

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
	#version 330 core
	layout(points) in;
	layout(line_strip, max_vertices = 36) out;

	in int vColor[];
	in int vCulling[];
	in vec3 vPos[];

	out int gColor;
	out vec3 gNormal;
	out vec3 gPos;

	uniform int bfculling;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void AddQuad(vec4 center, vec4 dy, vec4 dx) {

		gPos = vec3(center + (dx - dy));
		gl_Position = center + (dx - dy);
		EmitVertex();

		gPos = vec3(center + (-dx - dy));
		gl_Position = center + (-dx - dy);
		EmitVertex();

		gPos = vec3(center + (-dx + dy));
		gl_Position = center + (-dx + dy);
		EmitVertex();

		gPos = vec3(center + (dx + dy));
		gl_Position = center + (dx + dy);
		EmitVertex();

		gPos = vec3(center + (dx - dy));
		gl_Position = center + (dx - dy);
		EmitVertex();

		EndPrimitive();
	}

	void main() {
		vec4 center = gl_in[0].gl_Position;
		mat4 mvp = (projection * view * model);

		vec4 dx = mvp[0] / 2.2f;
		vec4 dy = mvp[1] / 2.2f;
		vec4 dz = mvp[2] / 2.2f;

		int cullflags = vCulling[0] & bfculling;
		if(cullflags & 0x80){ //if the voxel is hidden

			//draw nothing

		} else {
			if(cullflags & 0x01){ //Xp
				gColor = vColor[0];
				gNormal = vec3(normalize(dx));
				AddQuad(center + dx, dy, dz);
			}

			if(cullflags & 0x02){ //Xn
				gColor = vColor[0];
				gNormal = vec3(normalize(-dx));
				AddQuad(center - dx, dz, dy);
			}

			if(cullflags & 0x04){ //Yp
				gColor = vColor[0];
				gNormal = vec3(normalize(dy));
				AddQuad(center + dy, dz, dx);
			}

			if(cullflags & 0x08){ //Yn
				gColor = vColor[0];
				gNormal = vec3(normalize(-dy));
				AddQuad(center - dy, dx, dz);
			}

			if(cullflags & 0x10){ //Zp
				gColor = vColor[0];
				gNormal = vec3(normalize(dz));
				AddQuad(center + dz, dx, dy);
			}

			if(cullflags & 0x20){ //Zn
				gColor = vColor[0];
				gNormal = vec3(normalize(-dz));
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
