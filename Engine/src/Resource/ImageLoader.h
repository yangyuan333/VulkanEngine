#pragma once

#include <filesystem>
#include <fstream>
#include <array>

#include "../Utility/RenderEnum.h"

namespace VulkanEngine
{
    struct ImageData
    {
        std::vector<uint8_t> ByteData;
        Format ImageFormat = Format::UNDEFINED;
        uint32_t Width = 0;
        uint32_t Height = 0;
        std::vector<std::vector<uint8_t>> MipLevels;
    };

    struct CubemapData
    {
        std::array<std::vector<uint8_t>, 6> Faces;
        Format FaceFormat = Format::UNDEFINED;
        uint32_t FaceWidth = 0;
        uint32_t FaceHeight = 0;
    };

    class ImageLoader
    {
    public:
        static ImageData LoadImageFromFile(const std::string& filepath);
        static CubemapData LoadCubemapImageFromFile(const std::string& filepath);
    };
}