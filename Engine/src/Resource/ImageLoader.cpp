#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>

namespace VulkanEngine
{ 
    static bool IsDDSImage(const std::string& filepath)
    {
        std::filesystem::path filename{ filepath };
        return filename.extension() == ".dds";
    }

    static bool IsZLIBImage(const std::string& filepath)
    {
        std::filesystem::path filename{ filepath };
        return filename.extension() == ".zlib";
    }

    static Format DDSFormatToImageFormat(tinyddsloader::DDSFile::DXGIFormat format)
    {
        switch (format)
        {
        case tinyddsloader::DDSFile::DXGIFormat::Unknown:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Float:
            return Format::R32G32B32A32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_UInt:
            return Format::R32G32B32A32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_SInt:
            return Format::R32G32B32A32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Float:
            return Format::R32G32B32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_UInt:
            return Format::R32G32B32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_SInt:
            return Format::R32G32B32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float:
            return Format::R16G16B16A16_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm:
            return Format::R16G16B16A16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UInt:
            return Format::R16G16B16A16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SNorm:
            return Format::R16G16B16A16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SInt:
            return Format::R16G16B16A16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_Float:
            return Format::R32G32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_UInt:
            return Format::R32G32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_SInt:
            return Format::R32G32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G8X24_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::D32_Float_S8X24_UInt:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Float_X8X24_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::X32_Typeless_G8X24_UInt:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UNorm:
            return Format::A2R10G10B10_UNORM_PACK_32;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UInt:
            return Format::A2R10G10B10_UINT_PACK_32;
        case tinyddsloader::DDSFile::DXGIFormat::R11G11B10_Float:
            return Format::B10G11R11_UFLOAT_PACK_32;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm:
            return Format::R8G8B8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
            return Format::R8G8B8A8_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt:
            return Format::R8G8B8A8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm:
            return Format::R8G8B8A8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt:
            return Format::R8G8B8A8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float:
            return Format::R16G16_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm:
            return Format::R16G16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt:
            return Format::R16G16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm:
            return Format::R16G16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt:
            return Format::R16G16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::D32_Float:
            return Format::D32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Float:
            return Format::R32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_UInt:
            return Format::R32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_SInt:
            return Format::R32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R24G8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::D24_UNorm_S8_UInt:
            return Format::D24_UNORM_S8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R24_UNorm_X8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::X24_Typeless_G8_UInt:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm:
            return Format::R8G8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt:
            return Format::R8G8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm:
            return Format::R8G8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt:
            return Format::R8G8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R16_Float:
            return Format::R16_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::D16_UNorm:
            return Format::D16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm:
            return Format::R16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_UInt:
            return Format::R16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm:
            return Format::R16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_SInt:
            return Format::R16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm:
            return Format::R8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8_UInt:
            return Format::R8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm:
            return Format::R8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8_SInt:
            return Format::R8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::A8_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R1_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R9G9B9E5_SHAREDEXP:
            return Format::E5B9G9R9_UFLOAT_PACK_32;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_B8G8_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::G8R8_G8B8_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_SNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::B5G6R5_UNorm:
            return Format::B5G6R5_UNORM_PACK_16;
        case tinyddsloader::DDSFile::DXGIFormat::B5G5R5A1_UNorm:
            return Format::B5G5R5A1_UNORM_PACK_16;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm:
            return Format::B8G8R8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10_XR_BIAS_A2_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB:
            return Format::B8G8R8A8_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm_SRGB:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC6H_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC6H_UF16:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC6H_SF16:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_Typeless:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::AYUV:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::Y410:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::Y416:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::NV12:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::P010:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::P016:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::YUV420_OPAQUE:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::YUY2:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::Y210:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::Y216:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::NV11:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::AI44:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::IA44:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::P8:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::A8P8:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::B4G4R4A4_UNorm:
            return Format::B4G4R4A4_UNORM_PACK_16;
        case tinyddsloader::DDSFile::DXGIFormat::P208:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::V208:
            return Format::UNDEFINED;
        case tinyddsloader::DDSFile::DXGIFormat::V408:
            return Format::UNDEFINED;
        default:
            return Format::UNDEFINED;
        }
    }

    static ImageData LoadImageUsingDDSLoader(const std::string& filepath)
    {
        ImageData image;
        tinyddsloader::DDSFile dds;
        auto result = dds.Load(filepath.c_str());
        if (result != tinyddsloader::Result::Success)
            return image;

        dds.Flip();
        auto imageData = dds.GetImageData();
        image.Width = imageData->m_height;
        image.Height = imageData->m_height;
        image.ImageFormat = DDSFormatToImageFormat(dds.GetFormat());
        image.ByteData.resize(imageData->m_memSlicePitch);
        std::copy((const uint8_t*)imageData->m_mem, (const uint8_t*)imageData->m_mem + imageData->m_memSlicePitch, image.ByteData.begin());

        for (uint32_t i = 1; i < dds.GetMipCount(); i++)
        {
            auto& mipLevelData = image.MipLevels.emplace_back();
            auto mipImageData = dds.GetImageData(i);
            mipLevelData.resize(mipImageData->m_memSlicePitch);
            std::copy((const uint8_t*)mipImageData->m_mem, (const uint8_t*)mipImageData->m_mem + mipImageData->m_memSlicePitch, mipLevelData.begin());
        }

        return image;
    }

    static std::string ConvertZLIBToDDS(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.good()) return filepath;

        std::vector<char> compressedData{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        int decompressedSize = 0;
        char* decompressedData = stbi_zlib_decode_malloc(compressedData.data(), (int)compressedData.size(), &decompressedSize);

        std::filesystem::path path{ filepath };
        auto ddsFilepath = (path.parent_path() / path.stem()).string() + ".dds";
        std::ofstream ddsFile(ddsFilepath, std::ios::binary);
        ddsFile.write(decompressedData, decompressedSize);
        free(decompressedData);

        return ddsFilepath;
    }

    static ImageData LoadImageUsingZLIBLoader(const std::string& filepath)
    {
        auto ddsFilepath = ConvertZLIBToDDS(filepath);
        return LoadImageUsingDDSLoader(ddsFilepath);
    }

    static ImageData LoadImageUsingSTBLoader(const std::string& filepath)
    {
        int width = 0, height = 0, channels = 0;
        const uint32_t actualChannels = 4;

        stbi_set_flip_vertically_on_load(true);
        uint8_t* data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        stbi_set_flip_vertically_on_load(false);

        std::vector<uint8_t> vecData;
        vecData.resize(width * height * actualChannels * sizeof(uint8_t));
        std::copy(data, data + vecData.size(), vecData.begin());
        stbi_image_free(data);
        return ImageData{ std::move(vecData), Format::R8G8B8A8_UNORM, (uint32_t)width, (uint32_t)height };
    }

    static std::vector<uint8_t> ExtractCubemapFace(const ImageData& image, size_t faceWidth, size_t faceHeight, size_t channelCount, size_t sliceX, size_t sliceY)
    {
        std::vector<uint8_t> result(image.ByteData.size() / 6);

        for (size_t i = 0; i < faceHeight; i++)
        {
            size_t y = (faceHeight - i - 1) + sliceY * faceHeight;
            size_t x = sliceX * faceWidth;
            size_t bytesInRow = faceWidth * channelCount;

            std::memcpy(result.data() + i * bytesInRow, image.ByteData.data() + (y * image.Width + x) * channelCount, bytesInRow);
        }
        return result;
    };

    static CubemapData CreateCubemapFromSingleImage(const ImageData& image)
    {
        CubemapData cubemapData;
        cubemapData.FaceFormat = image.ImageFormat;
        cubemapData.FaceWidth = image.Width / 4;
        cubemapData.FaceHeight = image.Height / 3;
        assert(cubemapData.FaceWidth == cubemapData.FaceHeight); // single face should be a square
        assert(cubemapData.FaceFormat == Format::R8G8B8A8_UNORM); // support only this format for now
        constexpr size_t ChannelCount = 4;

        cubemapData.Faces[0] = ExtractCubemapFace(image, cubemapData.FaceWidth, cubemapData.FaceHeight, ChannelCount, 2, 1);
        cubemapData.Faces[1] = ExtractCubemapFace(image, cubemapData.FaceWidth, cubemapData.FaceHeight, ChannelCount, 0, 1);
        cubemapData.Faces[2] = ExtractCubemapFace(image, cubemapData.FaceWidth, cubemapData.FaceHeight, ChannelCount, 1, 2);
        cubemapData.Faces[3] = ExtractCubemapFace(image, cubemapData.FaceWidth, cubemapData.FaceHeight, ChannelCount, 1, 0);
        cubemapData.Faces[4] = ExtractCubemapFace(image, cubemapData.FaceWidth, cubemapData.FaceHeight, ChannelCount, 1, 1);
        cubemapData.Faces[5] = ExtractCubemapFace(image, cubemapData.FaceWidth, cubemapData.FaceHeight, ChannelCount, 3, 1);
        return cubemapData;

    }

    // void ImageLoader::FillImage(CommandBuffer& commandBuffer, Image& image, const ImageData& imageData, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryUsage, ImageOptions::Value options)
    // {
	// 	image.Init(
	// 		imageData.Width, imageData.Height, usage, memoryUsage, options);
    // }

    ImageData VulkanEngine::ImageLoader::LoadImageFromFile(const std::string& filepath)
	{
		if (IsDDSImage(filepath))
			return LoadImageUsingDDSLoader(filepath);
		else if (IsZLIBImage(filepath))
			return LoadImageUsingZLIBLoader(filepath);
		else
			return LoadImageUsingSTBLoader(filepath); // only see it
		return ImageData();
	}

	CubemapData VulkanEngine::ImageLoader::LoadCubemapImageFromFile(const std::string& filepath)
	{
        auto imageData = ImageLoader::LoadImageFromFile(filepath);
        return CreateCubemapFromSingleImage(imageData);
	}
}