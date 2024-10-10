#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;  

layout(location = 0) out vec4 gbufferA;
layout(location = 1) out vec2 gbufferB;
layout(location = 2) out vec4 gbufferC;
layout(location = 3) out vec4 gbufferD;

// struct Material
// {   
//     vec3 albedo;
//     vec2 metallicRoughness;
//     float roughnessScale;
//     float metallicScale;
// };

layout(set = 0, binding = 1) uniform sampler2D albedoTextureSampler;
layout(set = 0, binding = 2) uniform sampler2D normalTextureSampler;
layout(set = 0, binding = 3) uniform sampler2D metallicRoughnessTextureSampler;

// layout(set = 0, binding = 4) uniform MaterialBuffer {
//     Material material;
// };

void main() {
	vec3 albedoColor = texture(albedoTextureSampler, vin.texcoord).rgb; // * material.albedo;
	vec3 normal = texture(normalTextureSampler, vin.texcoord).rgb;
    normal = vin.tangentBasis * (2 * normal - 1.0);
    vec2 metallicRoughness = texture(metallicRoughnessTextureSampler, vin.texcoord).rg; // * material.metallicRoughness;     

    gbufferA = vec4(vin.position, 1.0); // 后面把它给优化掉, 换成 Motion Vector or AO
    gbufferB = vec2(normal.r, normal.g);
    gbufferC = vec4(albedoColor, 1.0);
    gbufferD = vec4(metallicRoughness, 0.0, 1.0);
}