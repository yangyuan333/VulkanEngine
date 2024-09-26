#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/simd/matrix.h>

#include <vector>
#include <filesystem>

#include "ImageLoader.h"
#include "../Render/GameObject.h"

namespace VulkanEngine
{
    struct ModelData
    {
        // struct Vertex
        // {
        //     glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
        //     glm::vec2 TexCoord{ 0.0f, 0.0f };
        //     glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
        //     glm::vec3 Tangent{ 0.0f, 0.0f, 0.0f };
        //     glm::vec3 Bitangent{ 0.0f, 0.0f, 0.0f };
        // };

        struct Material
        {
            std::string Name;
            ImageData AlbedoTexture;
            ImageData NormalTexture;
            ImageData MetallicRoughness;
            float RoughnessScale = 1.0f;
            float MetallicScale = 1.0f;
        };

        using Index = uint32_t;

        struct Shape
        {
            std::string Name;
            std::vector<Vertex> Vertices;
            std::vector<Index> Indices;
            uint32_t MaterialIndex = -1;
        };

        std::vector<Shape> Shapes; // Not one-to-one correspondence.
        std::vector<Material> Materials;
    };
	class ModelLoader
	{
	public:
		static ModelData LoadFromObj(const std::string& filepath, const std::string& metallicRoughnessPath = "");
		static ModelData LoadFromGltf(const std::string& filepath);
		static ModelData Load(const std::string& filepath, const std::string& metallicRoughnessPath = "");
    private:
        static constexpr glm::uvec4 DefaultAlbedo = glm::uvec4{255,255,255,255};
        static constexpr glm::uvec3 DefaultNormal = glm::uvec3{127,127,255};
        static constexpr glm::uvec2 DefaultMetallicRoughness = glm::uvec2{0,255};
	};
}