#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) out vec2 uv;

const vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

const vec2 uvs[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 2.0),
    vec2(2.0, 0.0)
);

layout(set = 1, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 viewPos;
} cameraData;

void main() {
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	uv = uvs[gl_VertexIndex];
}