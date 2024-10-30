#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(std140, set = 0, binding = 0) uniform ModelBuffer {   
    mat4 model;
	mat4 model_it;
} modelData;

layout(set = 1, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 viewPos;
} cameraData;

layout(location = 0) out Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vout;

void main() {
	vec4 world_position = modelData.model * vec4(position, 1.0); 
	vout.position = world_position.xyz;
	gl_Position = cameraData.viewproj * world_position;
    vout.texcoord = texCoord;

	vec3 world_tangent = mat3(modelData.model_it) * tangent;
	vec3 world_normal = mat3(modelData.model_it) * normal;

	vec3 world_bitangent = cross(world_normal, world_tangent);
    vout.tangentBasis = mat3(world_tangent, world_bitangent, world_normal);
}