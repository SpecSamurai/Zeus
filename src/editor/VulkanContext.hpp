#pragma once

#include "Camera.hpp"
#include "gltf/GLTFLoader.hpp"
#include "vulkan/DeletionQueue.hpp"
#include "vulkan/DescriptorAllocator.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/ImageBuilder.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/SwapchainBuilder.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_types.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <cstdint>
#include <span>
#include <vector>

namespace Zeus
{
inline std::vector<std::shared_ptr<MeshAsset>> testMeshes;
inline std::vector<ComputeEffect> backgroundEffects;
inline int currentBackgroundEffect{ 0 };

inline GPUSceneData sceneData;
inline VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

class VulkanContext
{
public:
    VulkanContext()
    {
        camera =
            Camera(Vector3f(0.f, 0.f, 0.f), Vector3f(0.0f, 1.0f, 0.0f), -90.f);
    }

    void init(std::uint32_t width, std::uint32_t height, const char* title);
    void run();
    void destroy();

    // private:
    void draw();

    void initWindow(int width, int height, const char* title);
    void initInstance();
    void initDevice();
    void initMemoryAllocator();
    void initSwapchain();
    void initDrawResources();

    void initCommands();
    void initSynchronizationObjects();

    void initPipelines();

    void initImGui();

    void resizeSwapchain();

    void updateUniformBuffer(std::uint32_t currentImage);
    void recordCommandBuffer(
        VkCommandBuffer commandBuffer,
        uint32_t imageIndex);

    Image VulkanContext::create_image(
        void* data,
        VkExtent3D size,
        VkFormat format,
        VkImageUsageFlags usage,
        bool mipmapped);
    // todo: consider using span instaed of ptr + size

    void initDescriptorSets();
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
    void transitionImageLayout(
        VkImage image,
        [[maybe_unused]] VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        uint32_t mipLevels);

    void createTextureSampler();

    void createTextureImage();

    void copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height);

    void cmdCopyBuffer(
        const VkBuffer& srcBuffer,
        const VkBuffer& dstBuffer,
        const VkDeviceSize& size);

    void generateMipmaps(
        VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        uint32_t mipLevels);

    void cmdOneTimeSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

    static void glfwKeyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods);

    static void glfwCursorPositionCallback(
        GLFWwindow* window,
        double xpos,
        double ypos);

    static void glfwScrollCallback(
        GLFWwindow* window,
        double xoffset,
        double yoffset);

    static void glfwWindowIconifyCallback(GLFWwindow* window, int iconified);

    static void framebufferResizeCallback(
        GLFWwindow* window,
        int width,
        int height);

    GPUMeshBuffers uploadMesh(
        std::span<uint32_t> indices,
        std::span<Vertex> vertices);

    uint32_t packUnorm4x8(float r, float g, float b, float a)
    {
        // Clamp the input values between 0 and 1
        r = r < 0 ? 0 : (r > 1 ? 1 : r);
        g = g < 0 ? 0 : (g > 1 ? 1 : g);
        b = b < 0 ? 0 : (b > 1 ? 1 : b);
        a = a < 0 ? 0 : (a > 1 ? 1 : a);

        // Convert to 8-bit values
        uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
        uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
        uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
        uint8_t a8 = static_cast<uint8_t>(a * 255.0f);

        // Pack the values into a single 32-bit integer
        return (a8 << 24) | (b8 << 16) | (g8 << 8) | r8;
    }

    void init_default_data()
    {
        testMeshes = loadGltfMeshes(
                         [this](
                             std::span<uint32_t> indices,
                             std::span<Vertex> vertices) {
                             return uploadMesh(indices, vertices);
                         },
                         "D:/Code/Zeus/src/editor/assets/models/basicmesh.glb")
                         .value();

        // _mainDeletionQueue.push_function([&]() {
        //     destroy_buffer(rectangle.indexBuffer);
        //     destroy_buffer(rectangle.vertexBuffer);
        // });

        // 3 default textures, white, grey, black. 1 pixel each
        uint32_t white = packUnorm4x8(1, 1, 1, 1);
        _whiteImage = create_image(
            (void*)&white,
            VkExtent3D{ 1, 1, 1 },
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_USAGE_SAMPLED_BIT,
            false);

        uint32_t
            grey; // = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
        _greyImage = create_image(
            (void*)&grey,
            VkExtent3D{ 1, 1, 1 },
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_USAGE_SAMPLED_BIT,
            false);

        uint32_t black = packUnorm4x8(0, 0, 0, 1);
        _blackImage = create_image(
            (void*)&black,
            VkExtent3D{ 1, 1, 1 },
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_USAGE_SAMPLED_BIT,
            false);

        // checkerboard image
        uint32_t magenta = packUnorm4x8(1, 0, 1, 1);
        std::array<uint32_t, 16 * 16> pixels; // for 16x16 checkerboard texture
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }
        _errorCheckerboardImage = create_image(
            pixels.data(),
            VkExtent3D{ 16, 16, 1 },
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_USAGE_SAMPLED_BIT,
            false);

        VkSamplerCreateInfo sampl = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO
        };

        sampl.magFilter = VK_FILTER_NEAREST;
        sampl.minFilter = VK_FILTER_NEAREST;

        vkCreateSampler(
            device.logicalDevice,
            &sampl,
            nullptr,
            &_defaultSamplerNearest);

        sampl.magFilter = VK_FILTER_LINEAR;
        sampl.minFilter = VK_FILTER_LINEAR;
        vkCreateSampler(
            device.logicalDevice,
            &sampl,
            nullptr,
            &_defaultSamplerLinear);

        // We have the option to either use
        // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, which packages an image
        // and a sampler to use with that image, or to use 2 descriptors, and
        // separate the two into VK_DESCRIPTOR_TYPE_SAMPLER and
        // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE. According to gpu vendors, the
        // separated approach can be faster as there is less duplicated data.
        // But its a bit harder to deal with so we wont be doing it for now.
        // Instead, we will use the combined descriptor to make our shaders
        // simpler.

        // _mainDeletionQueue.push_function([&]() {
        //     vkDestroySampler(_device, _defaultSamplerNearest, nullptr);
        //     vkDestroySampler(_device, _defaultSamplerLinear, nullptr);
        //
        //     destroy_image(_whiteImage);
        //     destroy_image(_greyImage);
        //     destroy_image(_blackImage);
        //     destroy_image(_errorCheckerboardImage);
        // });
    }

public:
    float lastX = 800 / 2.f;
    float lastY = 600 / 2.f;
    bool firstMouse = true;

    // *************************************************************
    DeletionQueue deletionQueue;
    DescriptorAllocator descriptorAllocator;
    SwapchainBuilder swapchainBuilder;
    ImageBuilder imageBuilder;

    // hmmmm
    Camera camera;

    static constexpr std::uint32_t MAX_CONCURRENT_FRAMES{ 2 };

    std::uint32_t currentFrame{ 0 };
    bool stopRendering{ false };

    VkExtent2D windowExtent;
    bool windowResized{ false };
    GLFWwindow* window{ nullptr };

    Instance instance;
    Device device;
    VmaAllocator allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };
    Swapchain swapchain;

    VkExtent2D drawExtent;
    float renderScale{ 1.f };
    Image drawImage;
    Image depthImage;

    FrameData frames[MAX_CONCURRENT_FRAMES];
    constexpr FrameData& getCurrentFrame();

    VkCommandPool immediateCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer immediateCommandBuffer{ VK_NULL_HANDLE };
    VkFence immediateSubmitFence{ VK_NULL_HANDLE };

    VkDescriptorPool imGuiDescriptorPool;

    // TODO: Rename
    VkDescriptorSet drawImageDescriptorSet;
    VkDescriptorSetLayout drawImageDescriptorLayout;
    //***************************************************************

    VkPipelineLayout _meshPipelineLayout;
    VkPipeline _meshPipeline;

    VkPipeline gradientPipeline;
    VkPipelineLayout gradientPipelineLayout;

    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };

    // In MSAA, each pixel is sampled in an offscreen buffer which is then
    // rendered to the screen. This new buffer is slightly different from
    // regular images we've been rendering to - they have to be able to store
    // more than one sample per pixel. Once a multisampled buffer is created, it
    // has to be resolved to the default framebuffer (which stores only a single
    // sample per pixel). This is why we have to create an additional render
    // target and modify our current drawing process. We only need one render
    // target since only one drawing operation is active at a time, just like
    // with the depth buffer

    VkImage colorImage;
    // VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
    VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };

    // uint32_t mipLevels;
    VkImage textureImage;
    VkImageView textureImageView;
    VkSampler textureSampler;

    Image _whiteImage;
    Image _blackImage;
    Image _greyImage;
    Image _errorCheckerboardImage;

    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;
    VkDescriptorSetLayout _singleImageDescriptorLayout;
};
}
