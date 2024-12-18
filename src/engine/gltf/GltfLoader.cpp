#include "GltfLoader.hpp"

#include "core/Engine.hpp"
#include "fastgltf/tools.hpp"
#include "logging/logger.hpp"
#include "math/Quaternion.hpp"
#include "math/definitions.hpp"
#include "math/transformations.hpp"
#include "rendering/resources.hpp"
#include "rhi/DescriptorPool.hpp"
#include "rhi/Sampler.hpp"
#include "rhi/VkContext.hpp"
#include "utility.hpp"

#include <cstdint>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>
#include <optional>

namespace Zeus
{
GltfLoader& GltfLoader::GetInstance()
{
    static GltfLoader instance;
    return instance;
}

std::optional<std::shared_ptr<LoadedGLTF>> GltfLoader::Load(
    std::filesystem::path filePath)
{
    LOG_DEBUG("Loading GLTF asset: {}", filePath.string());

    std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();

    auto data{ fastgltf::GltfDataBuffer::FromPath(filePath) };
    if (data.error() != fastgltf::Error::None)
    {
        LOG_ERROR(
            "Failed to load GLTF asset. {}",
            fastgltf::getErrorMessage(data.error()));

        return std::nullopt;
    }

    auto asset{
        m_parser.loadGltf(data.get(), filePath.parent_path(), PARSER_OPTIONS)
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

    // we can stimate the descriptors we will need accurately
    std::vector<VkDescriptorPoolSize> sizes = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 30 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 30 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 }
    };

    // file.descriptorPool.init(engine->_device, gltf.materials.size(), sizes);
    scene->descriptorPool =
        DescriptorPool(asset->materials.size(), sizes, 0, filePath.string());

    for (fastgltf::Sampler& sampler : asset->samplers)
    {
        Sampler* newSampler{ new Sampler(
            toVkFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest)),
            toVkFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest)),
            toVkSamplerMipmapMode(
                sampler.minFilter.value_or(fastgltf::Filter::Nearest)),
            toVkSamplerAddressMode(sampler.wrapS)) };

        scene->samplers.push_back(newSampler->GetHandle());
    }

    // temporal arrays for all the objects to use while creating the GLTF data
    std::vector<std::shared_ptr<MeshAsset>> meshes;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<Image*> images;
    // std::vector<TextureID> imageIDs;
    std::vector<std::shared_ptr<Material>> materials;

    for (fastgltf::Image& image : asset->images)
    {
        std::optional<Image*> img = GltfLoader::LoadImage(asset.get(), image);

        if (img.has_value())
        {
            images.push_back(img.value());
            scene->images[image.name.c_str()] = img.value();
            // imageIDs.push_back(
            //
            // engine->texCache.AddTexture(
            //     materialResources.colorImage.imageView,
            //     materialResources.colorSampler, );
        }
        else
        {
            // we failed to load, so lets give the slot a default white
            // texture
            // to not completely break loading
            images.push_back(&Textures::DebugCheckerboard);
            LOG_ERROR("Failed to load GLTF Image:{}", image.name);
        }
    }

    //> load_buffer
    // create buffer to hold the material data
    // file.materialDataBuffer = engine->create_buffer(
    //     sizeof(GLTFMetallic_Roughness::MaterialConstants) *
    //         gltf.materials.size(),
    //     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    //     VMA_MEMORY_USAGE_CPU_TO_GPU);

    scene->materialDataBuffer = Buffer(
        filePath.string(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        sizeof(GLTFMetallic_Roughness::MaterialConstants) *
            asset->materials.size(),
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true);

    int data_index = 0;
    // GLTFMetallic_Roughness::MaterialConstants* sceneMaterialConstants =
    //     (GLTFMetallic_Roughness::MaterialConstants*)
    //         file.materialDataBuffer.info.pMappedData;

    GLTFMetallic_Roughness::MaterialConstants* sceneMaterialConstants =
        (GLTFMetallic_Roughness::MaterialConstants*)
            scene->materialDataBuffer.GetData();
    //< load_buffer

    //> load_material
    for (fastgltf::Material& mat : asset->materials)
    {
        std::shared_ptr<Material> newMat = std::make_shared<Material>();
        materials.push_back(newMat);
        scene->materials[mat.name.c_str()] = newMat;

        GLTFMetallic_Roughness::MaterialConstants constants;
        constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
        constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
        constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
        constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

        constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
        constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;

        MaterialAlphaMode passType = MaterialAlphaMode::Opaque;
        if (mat.alphaMode == fastgltf::AlphaMode::Blend)
        {
            passType = MaterialAlphaMode::Transparent;
        }

        GLTFMetallic_Roughness::MaterialResources materialResources;
        // default the material textures
        // (textures on a GLTF material are optional. If they arent set, they
        // generally default to white)
        materialResources.colorImage =
            &Textures::DebugCheckerboard; // engine->_whiteImage;
        materialResources.colorSampler = Samplers::DefaultLinear.GetHandle();
        materialResources.metalRoughImage =
            &Textures::DebugCheckerboard; // engine->_whiteImage;
        materialResources.metalRoughSampler =
            Samplers::DefaultLinear.GetHandle();

        // set the uniform buffer for the material data
        materialResources.dataBuffer = scene->materialDataBuffer.GetHandle();
        materialResources.dataBufferOffset =
            data_index * sizeof(GLTFMetallic_Roughness::MaterialConstants);
        // grab textures from gltf file
        if (mat.pbrData.baseColorTexture.has_value())
        {
            size_t img = asset
                             ->textures[mat.pbrData.baseColorTexture.value()
                                            .textureIndex]
                             .imageIndex.value();
            size_t sampler = asset
                                 ->textures[mat.pbrData.baseColorTexture.value()
                                                .textureIndex]
                                 .samplerIndex.value();

            materialResources.colorImage = images[img];
            materialResources.colorSampler = scene->samplers[sampler];
        }

        // constants.colorTexID = engine->texCache
        //                            .AddTexture(
        //                                materialResources.colorImage.imageView,
        //                                materialResources.colorSampler)
        //                            .Index;
        // constants.metalRoughTexID =
        //     engine->texCache
        //         .AddTexture(
        //             materialResources.metalRoughImage.imageView,
        //             materialResources.metalRoughSampler)
        //         .Index;

        // write material parameters to buffer
        sceneMaterialConstants[data_index] = constants;
        // build material
        *newMat = Engine::GetRenderer().metalRoughMaterial.write_material(
            VkContext::GetLogicalDevice(),
            passType,
            materialResources,
            scene->descriptorPool);

        data_index++;
    }

    // use the same vectors for all meshes so that the memory doesnt reallocate
    // as often
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    for (fastgltf::Mesh& mesh : asset->meshes)
    {
        std::shared_ptr<MeshAsset> newmesh = std::make_shared<MeshAsset>();
        meshes.push_back(newmesh);
        scene->meshes[mesh.name.c_str()] = newmesh;
        newmesh->name = mesh.name;

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives)
        {
            GeoSurface newSurface;
            newSurface.startIndex = (uint32_t)indices.size();
            newSurface.count =
                (uint32_t)asset->accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = vertices.size();

            // load indexes
            {
                fastgltf::Accessor& indexaccessor =
                    asset->accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexaccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(
                    asset.get(),
                    indexaccessor,
                    [&](std::uint32_t idx) {
                        indices.push_back(idx + initial_vtx);
                    });
            }

            // load vertex positions
            {
                fastgltf::Accessor& posAccessor =
                    asset
                        ->accessors[p.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(),
                    posAccessor,
                    [&](fastgltf::math::fvec3 v, size_t index) {
                        Vertex newvtx;
                        newvtx.position = Vector3f(v.x(), v.y(), v.z());
                        newvtx.normal = { 1, 0, 0 };
                        newvtx.color = Color(1.f);
                        newvtx.uvX = 0;
                        newvtx.uvY = 0;
                        vertices[initial_vtx + index] = newvtx;
                    });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(),
                    asset->accessors[(*normals).accessorIndex],
                    [&](fastgltf::math::fvec3 v, size_t index) {
                        vertices[initial_vtx + index].normal =
                            Vector3f(v.x(), v.y(), v.z());
                    });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                    asset.get(),
                    asset->accessors[(*uv).accessorIndex],
                    [&](fastgltf::math::fvec2 v, size_t index) {
                        vertices[initial_vtx + index].uvX = v.x();
                        vertices[initial_vtx + index].uvY = v.y();
                    });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                    asset.get(),
                    asset->accessors[(*colors).accessorIndex],
                    [&](fastgltf::math::fvec4 v, size_t index) {
                        vertices[initial_vtx + index].color =
                            Color(v.x(), v.y(), v.z(), v.w());
                    });
            }

            if (p.materialIndex.has_value())
            {
                newSurface.material = materials[p.materialIndex.value()];
            }
            else
            {
                newSurface.material = materials[0];
            }

            Vector3f minpos = vertices[initial_vtx].position;
            Vector3f maxpos = vertices[initial_vtx].position;
            for (int i = initial_vtx; i < vertices.size(); i++)
            {
                minpos = Zeus::min(minpos, vertices[i].position);
                maxpos = Zeus::max(maxpos, vertices[i].position);
            }

            newSurface.bounds.origin = (maxpos + minpos) / 2.f;
            newSurface.bounds.extents = (maxpos - minpos) / 2.f;
            newSurface.bounds.sphereRadius = length(newSurface.bounds.extents);

            newmesh->surfaces.push_back(newSurface);
        }

        newmesh->meshBuffers =
            Engine::GetRenderer().UploadMesh(vertices, indices);
    }

    //> load_nodes
    // load all nodes and their meshes
    for (fastgltf::Node& node : asset->nodes)
    {
        std::shared_ptr<Node> newNode;

        // find if the node has a mesh, and if it does hook it to the mesh
        // pointer and allocate it with the meshnode class
        if (node.meshIndex.has_value())
        {
            newNode = std::make_shared<MeshNode>();
            static_cast<MeshNode*>(newNode.get())->mesh =
                meshes[*node.meshIndex];
        }
        else
        {
            newNode = std::make_shared<Node>();
        }

        nodes.push_back(newNode);
        scene->nodes[node.name.c_str()];

        std::visit(
            fastgltf::visitor{
                [&](fastgltf::math::fmat4x4 matrix) {
                    memcpy(
                        &newNode->localTransform,
                        matrix.data(),
                        sizeof(matrix));
                },
                [&](fastgltf::TRS transform) {
                    Vector3f tl(
                        transform.translation[0],
                        transform.translation[1],
                        transform.translation[2]);
                    Quaternion rot(
                        transform.rotation.x(),
                        transform.rotation.y(),
                        transform.rotation.z(),
                        transform.rotation.w());
                    Vector3f sc(
                        transform.scale[0],
                        transform.scale[1],
                        transform.scale[2]);

                    Matrix4x4f tm = translation(tl);
                    Matrix4x4f rm = rotationMatrix4x4(rot);
                    Matrix4x4f sm = scale(sc);

                    newNode->localTransform = tm * rm * sm;
                },
            },
            node.transform);
    }

    // run loop again to setup transform hierarchy
    for (int i = 0; i < asset->nodes.size(); i++)
    {
        fastgltf::Node& node = asset->nodes[i];
        std::shared_ptr<Node>& sceneNode = nodes[i];

        for (auto& c : node.children)
        {
            sceneNode->children.push_back(nodes[c]);
            nodes[c]->parent = sceneNode;
        }
    }

    // find the top nodes, with no parents
    for (auto& node : nodes)
    {
        if (node->parent.lock() == nullptr)
        {
            scene->topNodes.push_back(node);
            node->refreshTransform(Matrix4x4f(1.f));
        }
    }

    return scene;
}

std::optional<Image*> GltfLoader::LoadImage(
    fastgltf::Asset& asset,
    fastgltf::Image& image)
{
    Image* newImage{ nullptr };

    int width, height, channels;

    auto createTexture2D{
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
            [&](fastgltf::sources::URI& filePath) {
                assert(
                    filePath.fileByteOffset == 0 &&
                    "STBI doesn't support offsets");
                assert(filePath.uri.isLocalPath());

                stbi_uc* data{ stbi_load(
                    filePath.uri.c_str(),
                    &width,
                    &height,
                    &channels,
                    STBI_rgb_alpha) };

                if (data)
                {
                    newImage = createTexture2D(
                        std::format("Image_Texture2D_URI_{}", image.name),
                        data,
                        width,
                        height);

                    stbi_image_free(data);
                }
                else
                    LOG_WARNING("Failed to load GLTF URI image:{}", image.name);
            },
            [&](fastgltf::sources::Array& array) {
                stbi_uc* data{ stbi_load_from_memory(
                    (stbi_uc const*)array.bytes.data(),
                    static_cast<int>(array.bytes.size()),
                    &width,
                    &height,
                    &channels,
                    STBI_rgb_alpha) };

                if (data)
                {
                    newImage = createTexture2D(
                        std::format("Image_Texture2D_Array_{}", image.name),
                        data,
                        width,
                        height);

                    stbi_image_free(data);
                }
                else
                    LOG_WARNING(
                        "Failed to load GLTF Array image:{}",
                        image.name);
            },
            [&](fastgltf::sources::BufferView& view) {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];

                std::visit(
                    fastgltf::visitor{
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
                                newImage = createTexture2D(
                                    std::format(
                                        "Image_Texture2D_BufferView_{}",
                                        image.name),
                                    data,
                                    width,
                                    height);

                                stbi_image_free(data);
                            }
                            else
                                LOG_WARNING(
                                    "Failed to load GLTF BufferView image:{}",
                                    image.name);
                        },
                    },
                    buffer.data);
            },
        },
        image.data);

    if (newImage != nullptr)
    {
        return newImage;
    }
    else
    {
        return std::nullopt;
    }
}
}
