#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in dvec3 inPosition;
layout(location = 2) in vec3 inColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}