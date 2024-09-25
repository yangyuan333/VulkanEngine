#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include <span>

namespace VulkanEngine
{ 
    /// <summary>
    ///  Utility Function
    /// </summary>
    /// <param name="filepath"></param>
    /// <returns></returns>
    static bool IsGLTFModel(const std::string& filepath)
    {
        std::filesystem::path filename{ filepath };
        return filename.extension() == ".gltf";
    }

    static bool IsObjModel(const std::string& filepath)
    {
        std::filesystem::path filename{ filepath };
        return filename.extension() == ".obj";
    }

    static std::string GetAbsolutePathToObjResource(const std::string& objPath, const std::string& relativePath)
    {
        const auto parentPath = std::filesystem::path{ objPath }.parent_path();
        const auto absolutePath = parentPath / relativePath;
        return absolutePath.string();
    }

    static ImageData CreateStubTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        return ImageData{ std::vector{ r, g, b, a }, Format::R8G8B8A8_UNORM, 1, 1, {} };
    }

    inline std::pair<glm::vec3, glm::vec3> ComputeTangentSpace(
        const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
        const glm::vec2& tex1, const glm::vec2& tex2, const glm::vec2& tex3
    )
    {
        // Edges of the triangle : postion delta
        auto deltaPos1 = pos2 - pos1;
        auto deltaPos2 = pos3 - pos1;

        // texture delta
        auto deltaT1 = tex2 - tex1;
        auto deltaT2 = tex3 - tex1;

        float r = 1.0f / (deltaT1.x * deltaT2.y - deltaT1.y * deltaT2.x);
        auto tangent = (deltaPos1 * deltaT2.y - deltaPos2 * deltaT1.y) * r;
        auto bitangent = (deltaPos2 * deltaT1.x - deltaPos1 * deltaT2.x) * r;

        return std::make_pair(glm::normalize(tangent), glm::normalize(bitangent));
    }

    static auto ComputeTangentsBitangents(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib)
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> tangentsBitangents;
        tangentsBitangents.resize(attrib.normals.size(), { glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f } });

        ModelData::Index indexOffset = 0;
        for (size_t faceIndex : mesh.num_face_vertices)
        {
            assert(faceIndex == 3);

            tinyobj::index_t idx1 = mesh.indices[indexOffset + 0];
            tinyobj::index_t idx2 = mesh.indices[indexOffset + 1];
            tinyobj::index_t idx3 = mesh.indices[indexOffset + 2];

            glm::vec3 position1, position2, position3;
            glm::vec2 texCoord1, texCoord2, texCoord3;

            position1.x = attrib.vertices[3 * size_t(idx1.vertex_index) + 0];
            position1.y = attrib.vertices[3 * size_t(idx1.vertex_index) + 1];
            position1.z = attrib.vertices[3 * size_t(idx1.vertex_index) + 2];

            position2.x = attrib.vertices[3 * size_t(idx2.vertex_index) + 0];
            position2.y = attrib.vertices[3 * size_t(idx2.vertex_index) + 1];
            position2.z = attrib.vertices[3 * size_t(idx2.vertex_index) + 2];

            position3.x = attrib.vertices[3 * size_t(idx3.vertex_index) + 0];
            position3.y = attrib.vertices[3 * size_t(idx3.vertex_index) + 1];
            position3.z = attrib.vertices[3 * size_t(idx3.vertex_index) + 2];

            texCoord1.x = attrib.texcoords[2 * size_t(idx1.texcoord_index) + 0];
            texCoord1.y = attrib.texcoords[2 * size_t(idx1.texcoord_index) + 1];

            texCoord2.x = attrib.texcoords[2 * size_t(idx2.texcoord_index) + 0];
            texCoord2.y = attrib.texcoords[2 * size_t(idx2.texcoord_index) + 1];

            texCoord3.x = attrib.texcoords[2 * size_t(idx3.texcoord_index) + 0];
            texCoord3.y = attrib.texcoords[2 * size_t(idx3.texcoord_index) + 1];

            auto tangentBitangent = ComputeTangentSpace(
                position1, position2, position3,
                texCoord1, texCoord2, texCoord3
            );

            tangentsBitangents[idx1.vertex_index].first += tangentBitangent.first;
            tangentsBitangents[idx1.vertex_index].second += tangentBitangent.second;

            tangentsBitangents[idx2.vertex_index].first += tangentBitangent.first;
            tangentsBitangents[idx2.vertex_index].second += tangentBitangent.second;

            tangentsBitangents[idx3.vertex_index].first += tangentBitangent.first;
            tangentsBitangents[idx3.vertex_index].second += tangentBitangent.second;

            indexOffset += faceIndex;
        }

        for (auto& [tangent, bitangent] : tangentsBitangents)
        {
            if (tangent != glm::vec3{ 0.0f, 0.0f, 0.0f }) tangent = glm::normalize(tangent);
            if (bitangent != glm::vec3{ 0.0f, 0.0f, 0.0f }) bitangent = glm::normalize(bitangent);
        }
        return tangentsBitangents;
    }

    static Format ImageFormatFromGLTFImage(const tinygltf::Image& image)
    {
        if (image.component == 1)
        {
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE)
                return Format::R8_SNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                return Format::R8_UNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT)
                return Format::R16_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                return Format::R16_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT)
                return Format::R32_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                return Format::R32_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT)
                return Format::R32_SFLOAT;
        }
        if (image.component == 2)
        {
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE)
                return Format::R8G8_SNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                return Format::R8G8_UNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT)
                return Format::R16G16_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                return Format::R16G16_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT)
                return Format::R32G32_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                return Format::R32G32_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT)
                return Format::R32G32_SFLOAT;
        }
        if (image.component == 3)
        {
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE)
                return Format::R8G8B8_SNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                return Format::R8G8B8_UNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT)
                return Format::R16G16B16_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                return Format::R16G16B16_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT)
                return Format::R32G32B32_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                return Format::R32G32B32_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT)
                return Format::R32G32B32_SFLOAT;
        }
        if (image.component == 4)
        {
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE)
                return Format::R8G8B8A8_SNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                return Format::R8G8B8A8_UNORM;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT)
                return Format::R16G16B16A16_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                return Format::R16G16B16A16_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT)
                return Format::R32G32B32A32_SINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                return Format::R32G32B32A32_UINT;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT)
                return Format::R32G32B32A32_SFLOAT;
        }
        return Format::UNDEFINED;
    }

	static auto ComputeTangentsBitangents(std::span<ModelData::Index> indices,
		std::span<const glm::vec3>  positions,
		std::span<const glm::vec2>  texCoords) {
		std::vector<std::pair<glm::vec3, glm::vec3>> tangentsBitangents;
		tangentsBitangents.resize(positions.size(),
			{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f} });

		assert(indices.size() % 3 == 0);
		for (size_t i = 0; i < indices.size(); i += 3) {
			const auto& position1 = positions[indices[i + 0]];
			const auto& position2 = positions[indices[i + 1]];
			const auto& position3 = positions[indices[i + 2]];

			const auto& texCoord1 = texCoords[indices[i + 0]];
			const auto& texCoord2 = texCoords[indices[i + 1]];
			const auto& texCoord3 = texCoords[indices[i + 2]];

			auto tangentBitangent =
				ComputeTangentSpace(position1, position2, position3, texCoord1, texCoord2, texCoord3);

			tangentsBitangents[indices[i + 0]].first += tangentBitangent.first;
			tangentsBitangents[indices[i + 0]].second += tangentBitangent.second;

			tangentsBitangents[indices[i + 1]].first += tangentBitangent.first;
			tangentsBitangents[indices[i + 1]].second += tangentBitangent.second;

			tangentsBitangents[indices[i + 2]].first += tangentBitangent.first;
			tangentsBitangents[indices[i + 2]].second += tangentBitangent.second;
		}

		for (auto& [tangent, bitangent] : tangentsBitangents) {
			if (tangent != glm::vec3{ 0.0f, 0.0f, 0.0f }) tangent = glm::normalize(tangent);
			if (bitangent != glm::vec3{ 0.0f, 0.0f, 0.0f }) bitangent = glm::normalize(bitangent);
		}
		return tangentsBitangents;
	}

    /// <summary>
    ///  ModelLoader Function
    /// </summary>
    /// <param name="filepath"></param>
    /// <returns></returns>
	ModelData VulkanEngine::ModelLoader::LoadFromObj(const std::string& filepath, const std::string& metallicRoughnessPath)
	{
        ModelData result;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        result.Materials.reserve(materials.size());
		for (const auto& material : materials)
		{
			auto& resultMaterial = result.Materials.emplace_back();

			resultMaterial.Name = material.name;

			if (!material.diffuse_texname.empty())
				resultMaterial.AlbedoTexture = ImageLoader::LoadImageFromFile(GetAbsolutePathToObjResource(filepath, material.diffuse_texname));
			else
				resultMaterial.AlbedoTexture = CreateStubTexture(DefaultAlbedo.r, DefaultAlbedo.g, DefaultAlbedo.b, DefaultAlbedo.a);

			if (!material.normal_texname.empty())
				resultMaterial.NormalTexture = ImageLoader::LoadImageFromFile(GetAbsolutePathToObjResource(filepath, material.normal_texname));
			else
				resultMaterial.NormalTexture = CreateStubTexture(DefaultNormal.r, DefaultNormal.g, DefaultNormal.b);

			// not supported by obj format
			if (metallicRoughnessPath != "")
				resultMaterial.MetallicRoughness = ImageLoader::LoadImageFromFile(metallicRoughnessPath);
			else
				resultMaterial.MetallicRoughness = CreateStubTexture(DefaultMetallicRoughness.r, DefaultMetallicRoughness.g, DefaultMetallicRoughness.r, DefaultMetallicRoughness.g);
		}

        result.Shapes.reserve(shapes.size());
        for (const auto& shape : shapes)
        {
            auto& resultShape = result.Shapes.emplace_back();

            resultShape.Name = shape.name;

            if (!shape.mesh.material_ids.empty())
                resultShape.MaterialIndex = shape.mesh.material_ids.front();

            std::vector<std::pair<glm::vec3, glm::vec3>> tangentsBitangents;
            if (!attrib.normals.empty() && !attrib.texcoords.empty())
                tangentsBitangents = ComputeTangentsBitangents(shape.mesh, attrib);

            resultShape.Vertices.reserve(shape.mesh.indices.size());
            resultShape.Indices.reserve(shape.mesh.indices.size());

            ModelData::Index indexOffset = 0;
            for (size_t faceIndex : shape.mesh.num_face_vertices)
            {
                assert(faceIndex == 3); // only process 3 triangle
                for (size_t v = 0; v < faceIndex; v++, indexOffset++)
                {
                    tinyobj::index_t idx = shape.mesh.indices[indexOffset];
                    auto& vertex = resultShape.Vertices.emplace_back();
                    resultShape.Indices.push_back(indexOffset);

                    vertex.Position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.Position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.Position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    vertex.TexCoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.TexCoord.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                    vertex.Normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vertex.Normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vertex.Normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];

                    if (!tangentsBitangents.empty())
                    {
                        vertex.Tangent = tangentsBitangents[idx.vertex_index].first;
                        vertex.Bitangent = tangentsBitangents[idx.vertex_index].second;
                    }
                }
            }
        }
        return result;
	}

    ModelData ModelLoader::LoadFromGltf(const std::string& filepath)
    {
        ModelData result;

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string errorMessage, warningMessage;

        bool res = loader.LoadASCIIFromFile(&model, &errorMessage, &warningMessage, filepath);
        if (!res) return result;

        result.Materials.reserve(model.materials.size());
        for (const auto& material : model.materials)
        {
            auto& resultMaterial = result.Materials.emplace_back();

            resultMaterial.Name = material.name;
            resultMaterial.RoughnessScale = material.pbrMetallicRoughness.roughnessFactor;
            resultMaterial.MetallicScale = material.pbrMetallicRoughness.metallicFactor;

            if (material.pbrMetallicRoughness.baseColorTexture.index != -1)
            {
                const auto& albedoTexture = model.images[model.textures[material.pbrMetallicRoughness.baseColorTexture.index].source];
                resultMaterial.AlbedoTexture = ImageData{
                    albedoTexture.image,
                    ImageFormatFromGLTFImage(albedoTexture),
                    (uint32_t)albedoTexture.width,
                    (uint32_t)albedoTexture.height,
                };
            }
            else
            {
                resultMaterial.AlbedoTexture = CreateStubTexture(DefaultAlbedo.r, DefaultAlbedo.g, DefaultAlbedo.b, DefaultAlbedo.a);
            }

            if (material.normalTexture.index != -1)
            {
                const auto& normalTexture = model.images[model.textures[material.normalTexture.index].source];
                resultMaterial.NormalTexture = ImageData{
                    normalTexture.image,
                    ImageFormatFromGLTFImage(normalTexture),
                    (uint32_t)normalTexture.width,
                    (uint32_t)normalTexture.height,
                };
            }
            else
            {
                resultMaterial.NormalTexture = CreateStubTexture(DefaultNormal.r, DefaultNormal.g, DefaultNormal.b);
            }

            if (material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
            {
                const auto& metallicRoughnessTexture = model.images[model.textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index].source];
                resultMaterial.MetallicRoughness = ImageData{
                    metallicRoughnessTexture.image,
                    ImageFormatFromGLTFImage(metallicRoughnessTexture),
                    (uint32_t)metallicRoughnessTexture.width,
                    (uint32_t)metallicRoughnessTexture.height,
                };
            }
            else
            {
                resultMaterial.MetallicRoughness = CreateStubTexture(DefaultMetallicRoughness.r, DefaultMetallicRoughness.g, DefaultMetallicRoughness.r, DefaultMetallicRoughness.g);
            }
        }

        for (const auto& mesh : model.meshes)
        {
            result.Shapes.reserve(result.Shapes.size() + mesh.primitives.size());
            for (const auto& primitive : mesh.primitives)
            {
                auto& resultShape = result.Shapes.emplace_back();
                resultShape.Name = "shape_" + std::to_string(result.Shapes.size());
                resultShape.MaterialIndex = (uint32_t)primitive.material;

                const auto& indexAccessor = model.accessors[primitive.indices];
                auto& indexBuffer = model.bufferViews[indexAccessor.bufferView];
                const uint8_t* indexBegin = model.buffers[indexBuffer.buffer].data.data() + indexBuffer.byteOffset;

                const auto& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];

                const auto& positionBuffer = model.bufferViews[positionAccessor.bufferView];
                const auto& texCoordBuffer = model.bufferViews[texCoordAccessor.bufferView];
                const auto& normalBuffer = model.bufferViews[normalAccessor.bufferView];

                const uint8_t* positionBegin = model.buffers[positionBuffer.buffer].data.data() + positionBuffer.byteOffset;
                const uint8_t* texCoordBegin = model.buffers[texCoordBuffer.buffer].data.data() + texCoordBuffer.byteOffset;
                const uint8_t* normalBegin = model.buffers[normalBuffer.buffer].data.data() + normalBuffer.byteOffset;

                std::span<const glm::vec3> positions((const glm::vec3*)positionBegin, (const glm::vec3*)(positionBegin + positionBuffer.byteLength));
                std::span<const glm::vec2> texCoords((const glm::vec2*)texCoordBegin, (const glm::vec2*)(texCoordBegin + texCoordBuffer.byteLength));
                std::span<const glm::vec3> normals((const glm::vec3*)normalBegin, (const glm::vec3*)(normalBegin + normalBuffer.byteLength));

                if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
					std::span<const uint16_t> indices((const uint16_t*)indexBegin, (const uint16_t*)(indexBegin + indexBuffer.byteLength));
                    resultShape.Indices.resize(indices.size());
                    for (size_t i = 0; i < resultShape.Indices.size(); i++)
                        resultShape.Indices[i] = (ModelData::Index)indices[i];
                }
                if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
					std::span<const uint32_t> indices((const uint32_t*)indexBegin, (const uint32_t*)(indexBegin + indexBuffer.byteLength));
                    resultShape.Indices.resize(indices.size());
                    for (size_t i = 0; i < resultShape.Indices.size(); i++)
                        resultShape.Indices[i] = (ModelData::Index)indices[i];
                }

                auto tangentsBitangents = ComputeTangentsBitangents(resultShape.Indices, positions, texCoords);

                resultShape.Vertices.resize(positions.size());
                for (size_t i = 0; i < resultShape.Vertices.size(); i++)
                {
                    auto& vertex = resultShape.Vertices[i];
                    vertex.Position = positions[i];
                    vertex.TexCoord = texCoords[i];
                    vertex.Normal = normals[i];
                    vertex.Tangent = tangentsBitangents[i].first;
                    vertex.Bitangent = tangentsBitangents[i].second;
                }
            }
        }

        return result;
    }

	ModelData VulkanEngine::ModelLoader::Load(const std::string& filepath, const std::string& metallicRoughnessPath)
	{
		if (IsGLTFModel(filepath))
        {
            assert(metallicRoughnessPath == "");
			return ModelLoader::LoadFromGltf(filepath);
		}
        if (IsObjModel(filepath))
        { 
			return ModelLoader::LoadFromObj(filepath, metallicRoughnessPath);
		}
        assert(false); // unknown model format
		return ModelData{ };
	}
}