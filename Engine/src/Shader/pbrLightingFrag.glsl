#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec2 uv;

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

const float pointlightConst = 1.0;
const float pointlightLinear = 0.09;
const float pointlightQuat = 0.032;

const int MAX_LIGHT = 256;

struct Material {
    vec3 position;
	vec3 albedo;
	vec3 normal;
	float metallic;
	float roughness;
};

struct PointLight {
	vec3 position;
	vec3 intensity;
	vec3 lightColor;
};

layout (set = 0, binding = 0) uniform Sun {
	vec3 lightPos; // 为 shadow 做准备
    vec3 lightDirection; // cpu 端反向
	vec3 lightRadiance;
	vec3 lightColor;
} sun;

layout (set = 0, binding = 1) uniform PointLights {
	uint pointLightCnts;
	PointLight lights[MAX_LIGHT];
} pointLightArray;

layout(set = 1, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 viewPos;
} cameraData;

layout(input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput gbufferA;
layout(input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput gbufferB;
layout(input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput gbufferC;
layout(input_attachment_index = 3, set = 2, binding = 3) uniform subpassInput gbufferD;

layout (location = 0) out vec4 sceneColor;

vec3 GetNormal(vec2 normal_xy)
{
	float normal_z = sqrt(max(0.0, 1.0 - normal_xy.x * normal_xy.x - normal_xy.y * normal_xy.y));
	return vec3(normal_xy, normal_z);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 ComputePbr(Material material, vec3 lightDir, vec3 viewDir, vec3 lightradiance, vec3 F0)
{
	vec3 H = normalize(lightDir + viewDir);

	float NDF = DistributionGGX(material.normal, H, material.roughness);        
    float G   = GeometrySmith(material.normal, viewDir, lightDir, material.roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0); 

	vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;

	vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(material.normal, viewDir), 0.0) * max(dot(material.normal, lightDir), 0.0) + Epsilon; 
    vec3 specular     = nominator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(material.normal, lightDir), 0.0);                
    return (kD * material.albedo / PI + specular) * lightradiance * NdotL; 
}

void main() {
    Material material;

    material.albedo = subpassLoad(gbufferC).rgb;
    material.position = subpassLoad(gbufferA).rgb;
    material.normal = GetNormal(subpassLoad(gbufferB).rg);
	vec2 mr = subpassLoad(gbufferD).rg;
    material.metallic = mr.r;
    material.roughness = mr.g;

	vec3 eyePosition = cameraData.viewPos;	

	vec3 f0 = mix(Fdielectric, material.albedo, material.metallic);

	vec3 viewDir = normalize(eyePosition - material.position);

	vec3 pbrColor = ComputePbr(material, sun.lightDirection, viewDir, sun.lightRadiance, f0);

	vec3 pbrAmbilent = material.albedo * 0.1 * sun.lightColor;

	for(int i = 0; i < min(pointLightArray.pointLightCnts, MAX_LIGHT); ++i)
	{
		PointLight light = pointLightArray.lights[i];
		vec3 lightDir = normalize(light.position - material.position);
		float distance    = length(light.position - material.position);
        float attenuation = 1.0 / (pointlightConst + pointlightLinear * distance + pointlightQuat * (distance * distance));
        vec3 radiance     = light.intensity * light.lightColor * attenuation;  
		pbrColor += ComputePbr(material, lightDir, viewDir, radiance, f0);
	}

	sceneColor = vec4(pbrColor + pbrAmbilent, 1.0);
}