#include "GltfLoader.hpp"

#include <logging/logger.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>
#include <optional>

namespace Zeus
{
std::optional<bool> GltfLoader::Load(std::filesystem::path path)
{
    LOG_DEBUG("Loading GLTF asset: {}", path.string());

    auto data{ fastgltf::GltfDataBuffer::FromPath(path) };
    if (data.error() != fastgltf::Error::None)
    {
        LOG_ERROR(
            "Failed to load GLTF asset. {}",
            fastgltf::getErrorMessage(data.error()));

        return std::nullopt;
    }

    auto asset{
        m_parser.loadGltf(data.get(), path.parent_path(), PARSER_OPTIONS)
    };

    if (asset.error() != fastgltf::Error::None)
    {
        LOG_ERROR(
            "Failed to parse GLTF asset. {}",
            fastgltf::getErrorMessage(asset.error()));
        return std::nullopt;
    }

#ifndef NDEBUG
    auto validation{ fastgltf::validate(asset.get()) };
    if (validation != fastgltf::Error::None)
    {
        LOG_ERROR(
            "GLTF validation failed. {}",
            fastgltf::getErrorMessage(validation));
    }
#endif

    // call asset.get(), asset.get_if() or asset->
    for (auto& buffer : asset->buffers)
    {
        // Process the buffers.
    }

    return true;
}

std::optional<Image*> load_image(fastgltf::Asset& asset, fastgltf::Image& image)
{
    Image* newImage{ nullptr };

    int width, height, channels;

    auto createImage{
        [](std::string_view name, stbi_uc* data, int width, int height) {
            Image* image{
                new Image(
                    ImageType::Texture2D,
                    {
                        .width = static_cast<std::uint32_t>(width),
                        .height = static_cast<std::uint32_t>(height),
                        .depth = 1,
                    },
                    VK_FORMAT_R8G8B8A8_UNORM,
                    VK_IMAGE_USAGE_SAMPLED_BIT |
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    name),
            };

            image->Update(
                data,
                static_cast<std::size_t>(width * height * STBI_rgb_alpha),
                0);

            return image;
        }
    };

    std::visit(
        fastgltf::visitor{
            [](auto&) {},
            // Images from external files
            [&](fastgltf::sources::URI& filePath) {
                assert(
                    filePath.fileByteOffset == 0 &&
                    "STBI doesn't support offsets");
                assert(filePath.uri.isLocalPath());

                /*const std::string path(*/
                /*    filePath.uri.path().begin(),*/
                /*    filePath.uri.path().end()); // Thanks C++.*/
                stbi_uc* data{ stbi_load(
                    filePath.uri.c_str(),
                    &width,
                    &height,
                    &channels,
                    STBI_rgb_alpha) };

                if (data)
                {
                    newImage = createImage(
                        std::format("Image_{}", filePath.uri.path()),
                        data,
                        width,
                        height);

                    stbi_image_free(data);
                }
            },
            // base64 images or flag load external image files
            [&](fastgltf::sources::Array& vector) {
                stbi_uc* data{ stbi_load_from_memory(
                    (stbi_uc const*)vector.bytes.data(),
                    static_cast<int>(vector.bytes.size()),
                    &width,
                    &height,
                    &channels,
                    STBI_rgb_alpha) };
                if (data)
                {
                    newImage = createImage(
                        std::format("Image_{}", "Vaectorh"),
                        data,
                        width,
                        height);

                    stbi_image_free(data);
                }
            },
            // image file is embedded into the binary GLB file
            [&](fastgltf::sources::BufferView& view) {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];

                std::visit(
                    fastgltf::visitor{
                        // VectorWithMime here, because we specify
                        // LoadExternalBuffers, meaning all buffers are already
                        // loaded into a vector.
                        [](auto&) {},
                        [&](fastgltf::sources::Array& array) {
                            stbi_uc* bufferData{ reinterpret_cast<stbi_uc*>(
                                array.bytes.data() + bufferView.byteOffset) };

                            stbi_uc* data{ stbi_load_from_memory(
                                bufferData,
                                static_cast<int>(bufferView.byteLength),
                                &width,
                                &height,
                                &channels,
                                STBI_rgb_alpha) };
                            if (data)
                            {
                                newImage = createImage(
                                    std::format("Image_{}", "Array"),
                                    data,
                                    width,
                                    height);

                                stbi_image_free(data);
                            }
                        },
                    },
                    buffer.data);
            },
        },
        image.data);

    if (newImage != nullptr)
        return newImage;
    else
        return std::nullopt;
}
}
