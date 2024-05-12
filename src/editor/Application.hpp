#pragma once

#include "VulkanContext.hpp"
#include "vulkan_sync.hpp"
#define GLFW_INCLUDE_VULKAN

#include "glfw_utils.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>

// TODO: EX https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
// TRANSFER QUEUE

const std::string MODEL_PATH = "C:/Source/Zeus/textures/viking_room.obj";
const std::string TEXTURE_PATH = "C:/Source/Zeus/textures/viking_room.png";

namespace Zeus
{
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3>
    getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3>
            attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};

// We can exactly match the definition in the shader using data types in GLM.
// The data in the matrices is binary compatible with the way the shader expects
// it, so we can later just memcpy a UniformBufferObject to a VkBuffer.
struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

// const std::vector<Vertex> vertices = {
//     // {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     // {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//     // {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//     // {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
//     //
//     // {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     // {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//     // {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//     // {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
//
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//
//     {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//
//     {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//
//     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//
//     {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
//     {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
//     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//     {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}};
//
// const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

class Application
{
public:
    ~Application()
    {
        info("Terminating application");

        cleanupSwapChain();

        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

        vkDestroyRenderPass(device, renderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

        vkDestroySampler(device, textureSampler, nullptr);
        vkDestroyImageView(device, textureImageView, nullptr);

        vkDestroyImage(device, textureImage, nullptr);
        vkFreeMemory(device, textureImageMemory, nullptr);

        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);

        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, commandPool, nullptr);

        context.Destroy();

        // vkDestroyDevice(device, nullptr);

        // vkDestroySurfaceKHR(instance, surface, nullptr);
        // if (true)
        // {
        //     destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        // }
        // vkDestroyInstance(instance, nullptr);
        //
        // trace("Terminating window");
        //
        // glfwDestroyWindow(window);
        // glfwTerminate();
    }

    void Run()
    {
        // window.InitWindow(width, height, title);

        // window = createGlfwWindow(width, height, title);

        context.Init();

        window = context.window;
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        instance = context.instance;
        debugMessenger = context.debugUtilsMessenger;
        surface = context.surface;

        physicalDevice = context.device.physicalDevice;
        device = context.device.logicalDevice;
        graphicsQueue = context.device.graphicsQueue;
        presentQueue = context.device.presentQueue;
        msaaSamples = context.device.msaaSamples;

        InitVulkan();
        MainLoop();
    }

private:
    VulkanContext context;

    GLFWwindow* window;

    VkInstance instance{ VK_NULL_HANDLE };
    VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };

    VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
    VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };
    VkDevice device{ VK_NULL_HANDLE };

    VkQueue graphicsQueue{ VK_NULL_HANDLE };
    VkQueue presentQueue{ VK_NULL_HANDLE };

    VulkanDevice vulkanDevice;

    VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

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
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    // you should allocate multiple
    // resources like buffers from a single memory allocation, but in fact you
    // should go a step further. Driver developers recommend that you also store
    // multiple buffers, like the vertex and index buffer, into a single
    // VkBuffer and use offsets in commands like vkCmdBindVertexBuffers. The
    // advantage is that your data is more cache friendly in that case, because
    // it's closer together. It is even possible to reuse the same chunk of
    // memory for multiple resources if they are not used during the same render
    // operations, provided that their data is refreshed, of course. This is
    // known as aliasing and some Vulkan functions have explicit flags to
    // specify that you want to do this.
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::uint32_t currentFrame = 0;
    bool framebufferResized = false;

private:
    void MainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            DrawFrame();
        }
        // You can also wait for operations in a specific command queue to be
        // finished with vkQueueWaitIdle. These functions can be used as a very
        // rudimentary way to perform synchronization. You'll see that the
        // program now exits without problems when closing the window.
        vkDeviceWaitIdle(device);
    }

    static void framebufferResizeCallback(
        GLFWwindow* window,
        [[maybe_unused]] int width,
        [[maybe_unused]] int height)
    {
        auto app =
            reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
        info("Framebuffer resized");
        // added to make window draw while resizing
        app->DrawFrame();
    }

    void InitVulkan()
    {
        // createVkInstance(instance);
        // createVkDebugUtilsMessengerEXT(instance, debugMessenger);
        // createVkSurfaceKHR(instance, window, surface);

        // CreateLogicaDevice();

        CreateSwapChain();

        CreateRenderPass();
        createDescriptorSetLayout();
        CreateGraphicsPipeline(
            device,
            renderPass,
            descriptorSetLayout,
            pipelineLayout,
            graphicsPipeline,
            msaaSamples);
        CreateCommandPool();
        createColorResources();
        createDepthResources();
        CreateFramebuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        loadModel();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    void loadModel()
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(
                &attrib,
                &shapes,
                &materials,
                &warn,
                &err,
                MODEL_PATH.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        // std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos = { attrib.vertices[static_cast<size_t>(
                                   3 * index.vertex_index + 0)],
                               attrib.vertices[static_cast<size_t>(
                                   3 * index.vertex_index + 1)],
                               attrib.vertices[static_cast<size_t>(
                                   3 * index.vertex_index + 2)] };

                vertex.texCoord = { attrib.texcoords[static_cast<size_t>(
                                        2 * index.texcoord_index + 0)],
                                    1.0f - attrib.texcoords[static_cast<size_t>(
                                               2 * index.texcoord_index + 1)] };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                vertices.push_back(vertex);
                indices.push_back(static_cast<uint32_t>(indices.size()));

                // if (uniqueVertices.count(vertex) == 0)
                // {
                //     uniqueVertices[vertex] =
                //         static_cast<uint32_t>(vertices.size());
                //     vertices.push_back(vertex);
                // }
                //
                // indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void createColorResources()
    {
        VkFormat colorFormat = swapChainImageFormat;

        // We're also using only one mip level, since this is enforced by the
        // Vulkan specification in case of images with more than one sample per
        // pixel. Also, this color buffer doesn't need mipmaps since it's not
        // going to be used as a texture:
        createImage(
            swapChainExtent.width,
            swapChainExtent.height,
            1,
            msaaSamples,
            colorFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            colorImage,
            colorImageMemory);

        createVkImageView(
            device,
            colorImage,
            colorFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            colorImageView);
    }

    void createDepthResources()
    {
        VkFormat depthFormat = findDepthFormat();

        createImage(
            swapChainExtent.width,
            swapChainExtent.height,
            1,
            msaaSamples,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImage,
            depthImageMemory);

        createVkImageView(
            device,
            depthImage,
            depthFormat,
            VK_IMAGE_ASPECT_DEPTH_BIT,
            1,
            depthImageView);

        // optional
        // We don't need to explicitly transition the layout of the image to a
        // depth attachment because we'll take care of this in the render pass.
        // transitionImageLayout(
        //     depthImage,
        //     depthFormat,
        //     VK_IMAGE_LAYOUT_UNDEFINED,
        //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }

    VkFormat findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR &&
                (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (
                tiling == VK_IMAGE_TILING_OPTIMAL &&
                (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat findDepthFormat()
    {
        return findSupportedFormat(
            {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
            },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    bool hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
               format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void createTextureSampler()
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void createTextureImageView()
    {
        createVkImageView(
            device,
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels,
            textureImageView);
    }

    VkCommandBuffer beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void createTextureImage()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(
            TEXTURE_PATH.c_str(),
            &texWidth,
            &texHeight,
            &texChannels,
            STBI_rgb_alpha);

        // The pixels are laid out row by row with 4 bytes per pixel
        VkDeviceSize imageSize = (VkDeviceSize)(texWidth * texHeight * 4);
        mipLevels = static_cast<uint32_t>(
                        std::floor(std::log2(std::max(texWidth, texHeight)))) +
                    1;

        if (!pixels)
        {
            fatal("Failed to load texture image");
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(
            (std::uint32_t)texWidth,
            (std::uint32_t)texHeight,
            mipLevels,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage,
            textureImageMemory);

        transitionImageLayout(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mipLevels);
        copyBufferToImage(
            stagingBuffer,
            textureImage,
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight));
        // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while
        // generating mipmaps
        // transitionImageLayout(
        //     textureImage,
        //     VK_FORMAT_R8G8B8A8_SRGB,
        //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        generateMipmaps(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            texWidth,
            texHeight,
            mipLevels);
    }

    void generateMipmaps(
        VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        uint32_t mipLevels)
    {
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(
            physicalDevice,
            imageFormat,
            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures &
              VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error(
                "texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(
                commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,
                                   mipHeight > 1 ? mipHeight / 2 : 1,
                                   1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(
                commandBuffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        // Before we end the command buffer, we insert one more pipeline
        // barrier. This barrier transitions the last mip level from
        // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to
        // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL. This wasn't handled by the
        // loop, since the last mip level is never blitted from.
        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    void createImage(
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        VkSampleCountFlagBits numSamples,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
            findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    void createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount =
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount =
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(
                device,
                &poolInfo,
                nullptr,
                &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        // VkDescriptorPoolSize poolSize{};
        // poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // // Not fully correct! maxSets parameter specifies how many descriptor
        // // sets can be allocated from a given pool. But poolSizes parameter
        // // specifies the number of elements in the pPoolSizes array. And this
        // // array describes how many descriptors of a certain type will be
        // // allocated NOT in a single descriptor set but in total from a given
        // // pool. poolSize.descriptorCount parameter specifies the number of
        // // descriptors of a given type which can be allocated in total from a
        // // given pool (across all sets).
        // poolSize.descriptorCount =
        // static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        //
        // VkDescriptorPoolCreateInfo poolInfo{};
        // poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        // poolInfo.poolSizeCount = 1;
        // // But again - through the pPoolSizes array You specify the total
        // number
        // // of descriptors that can be allocated from a pool (not the total
        // // number of descriptors in a set or the total number of sets).
        // poolInfo.pPoolSizes = &poolSize;
        // poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        //
        // if (vkCreateDescriptorPool(
        //         device,
        //         &poolInfo,
        //         nullptr,
        //         &descriptorPool) != VK_SUCCESS)
        // {
        //     fatal("failed to create descriptor pool!");
        //     throw std::runtime_error("failed to create descriptor pool!");
        // }
    }

    void createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(
            MAX_FRAMES_IN_FLIGHT,
            descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount =
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(
                device,
                &allocInfo,
                descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImageView;
            imageInfo.sampler = textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType =
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(
                device,
                static_cast<uint32_t>(descriptorWrites.size()),
                descriptorWrites.data(),
                0,
                nullptr);
        }
    }

    void createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            CreateBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers[i],
                uniformBuffersMemory[i]);

            vkMapMemory(
                device,
                uniformBuffersMemory[i],
                0,
                bufferSize,
                0,
                &uniformBuffersMapped[i]);
        }
    }

    void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
            uboLayoutBinding,
            samplerLayoutBinding
        };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(
                device,
                &layoutInfo,
                nullptr,
                &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void CreateBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;

        // Just like the images in the swap chain, buffers can also be owned by
        // a specific queue family or be shared between multiple at the same
        // time. The buffer will only be used from the graphics queue, so we can
        // stick to exclusive access.
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        // The buffer has been created, but it doesn't actually have any memory
        // assigned to it yet. The first step of allocating memory for the
        // buffer is to query its memory requirements
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
            findMemoryType(memRequirements.memoryTypeBits, properties);

        // It should be noted that in a real world application, you're not
        // supposed to actually call vkAllocateMemory for every individual
        // buffer. The maximum number of simultaneous memory allocations is
        // limited by the maxMemoryAllocationCount physical device limit, which
        // may be as low as 4096 even on high end hardware like an NVIDIA GTX
        // 1080. The right way to allocate memory for a large number of objects
        // at the same time is to create a custom allocator that splits up a
        // single allocation among many different objects by using the offset
        // parameters that we've seen in many functions.

        // You can either implement such an allocator yourself, or use the
        // VulkanMemoryAllocator library provided by the GPUOpen initiative.
        // However, for this tutorial it's okay to use a separate allocation for
        // every resource, because we won't come close to hitting any of these
        // limits for now.
        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        // The first three parameters are self-explanatory and the fourth
        // parameter is the offset within the region of memory. Since this
        // memory is allocated specifically for this the vertex buffer, the
        // offset is simply 0. If the offset is non-zero, then it is required to
        // be divisible by memRequirements.alignment.
        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    void createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void createVertexBuffer()
    {
        // The flags parameter is used to configure sparse buffer memory, which
        // is not relevant right now. We'll leave it at the default value of 0.

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;

        // This function allows us to access a region of the specified memory
        // resource defined by an offset and size. The offset and size here are
        // 0 and bufferInfo.size, respectively. It is also possible to specify
        // the special value VK_WHOLE_SIZE to map all of the memory. The second
        // to last parameter can be used to specify flags, but there aren't any
        // available yet in the current API. It must be set to the value 0. The
        // last parameter specifies the output for the pointer to the mapped
        // memory.
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);

        // You can now simply memcpy the vertex data to the mapped memory and
        // unmap it again using vkUnmapMemory. Unfortunately the driver may not
        // immediately copy the data into the buffer memory, for example because
        // of caching. It is also possible that writes to the buffer are not
        // visible in the mapped memory yet. There are two ways to deal with
        // that problem:
        // Use a memory heap that is host coherent, indicated with
        // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT Call vkFlushMappedMemoryRanges
        // after writing to the mapped memory, and call
        // vkInvalidateMappedMemoryRanges before reading from the mapped memory
        // We went for the first approach, which ensures that the mapped memory
        // always matches the contents of the allocated memory. Do keep in mind
        // that this may lead to slightly worse performance than explicit
        // flushing, but we'll see why that doesn't matter
        // Flushing memory ranges or using a coherent memory heap means that the
        // driver will be aware of our writes to the buffer, but it doesn't mean
        // that they are actually visible on the GPU yet. The transfer of data
        // to the GPU is an operation that happens in the background and the
        // specification simply tells us that it is guaranteed to be complete as
        // of the next call to vkQueueSubmit.
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        // The vertex buffer we have right now works correctly, but the memory
        // type that allows us to access it from the CPU may not be the most
        // optimal memory type for the graphics card itself to read from. The
        // most optimal memory has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag
        // and is usually not accessible by the CPU on dedicated graphics cards.
        CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void transitionImageLayout(
        VkImage image,
        [[maybe_unused]] VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        uint32_t mipLevels)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        // barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format))
            {
                barrier.subresourceRange.aspectMask |=
                    VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    void copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        endSingleTimeCommands(commandBuffer);
    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);

        // VkCommandBufferAllocateInfo allocInfo{};
        // allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        // allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        // allocInfo.commandPool = commandPool;
        // allocInfo.commandBufferCount = 1;
        //
        // VkCommandBuffer commandBuffer;
        // vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
        //
        // VkCommandBufferBeginInfo beginInfo{};
        // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        //
        // vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // VkBufferCopy copyRegion{};
        // copyRegion.srcOffset = 0; // Optional
        // copyRegion.dstOffset = 0; // Optional
        // copyRegion.size = size;
        // vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        //
        // vkEndCommandBuffer(commandBuffer);
        //
        // VkSubmitInfo submitInfo{};
        // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        // submitInfo.commandBufferCount = 1;
        // submitInfo.pCommandBuffers = &commandBuffer;
        //
        // vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        // vkQueueWaitIdle(graphicsQueue);
        //
        // vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    // Graphics cards can offer different types of memory to allocate from. Each
    // type of memory varies in terms of allowed operations and performance
    // characteristics. We need to combine the requirements of the buffer and
    // our own application requirements to find the right type of memory to use.
    uint32_t findMemoryType(
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties)
    {
        // The VkPhysicalDeviceMemoryProperties structure has two arrays
        // memoryTypes and memoryHeaps. Memory heaps are distinct memory
        // resources like dedicated VRAM and swap space in RAM for when VRAM
        // runs out. The different types of memory exist within these heaps.
        // Right now we'll only concern ourselves with the type of memory and
        // not the heap it comes from, but you can imagine that this can affect
        // performance.
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            // The typeFilter parameter will be used to specify the bit field of
            // memory types that are suitable.That means that we can find the
            // index of a suitable memory type by simply iterating over them and
            // checking if the corresponding bit is set to 1.
            // However, we're not just interested in a memory type that is
            // suitable for the vertex buffer. We also need to be able to
            // write our vertex data to that memory. The memoryTypes array
            // consists of VkMemoryType structs that specify the heap and
            // properties of each type of memory. The properties define
            // special features of the memory, like being able to map it so
            // we can write to it from the CPU. This property is indicated
            // with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, but we also need to
            // use the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property.
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) ==
                    properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void cleanupSwapChain()
    {
        vkDestroyImageView(device, depthImageView, nullptr);
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);

        vkDestroyImageView(device, colorImageView, nullptr);
        vkDestroyImage(device, colorImage, nullptr);
        vkFreeMemory(device, colorImageMemory, nullptr);

        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void recreateSwapChain()
    {
        // this handles minimazation
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        // Note that we don't recreate the renderpass here for simplicity. In
        // theory it can be possible for the swap chain image format to change
        // during an applications' lifetime, e.g. when moving a window from an
        // standard range to an high dynamic range monitor. This may require the
        // application to recreate the renderpass to make sure the change
        // between dynamic ranges is properly reflected.
        vkDeviceWaitIdle(device);
        cleanupSwapChain();
        // createImageViews();
        CreateSwapChain();

        createColorResources();
        createDepthResources();
        CreateFramebuffers();
    }

    void updateUniformBuffer(uint32_t currentImage)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(
            glm::mat4(1.0f),
            time * glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(
            glm::radians(45.0f),
            (float)swapChainExtent.width / (float)swapChainExtent.height,
            0.1f,
            10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void DrawFrame()
    {
        vkWaitForFences(
            device,
            1,
            &inFlightFences[currentFrame],
            VK_TRUE,
            UINT64_MAX);

        std::uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            device,
            swapChain,
            UINT64_MAX,
            imageAvailableSemaphores[currentFrame],
            VK_NULL_HANDLE,
            &imageIndex);

        // You could also decide to do that if the swap chain is suboptimal, but
        // I've chosen to proceed anyway in that case because we've already
        // acquired an image.
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            info("AcquireNextImage: Recreating swapchain");
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(currentFrame);

        // Delay resetting the fence until after we know for sure we will be
        // submitting work with it. Thus, if we return early, the fence is still
        // signaled and vkWaitForFences wont deadlock the next time we use the
        // same fence object.
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        vkResetCommandBuffer(
            commandBuffers[currentFrame],
            /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
            imageAvailableSemaphores[currentFrame]
        };
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {
            renderFinishedSemaphores[currentFrame]
        };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(
                graphicsQueue,
                1,
                &submitInfo,
                inFlightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        // A pointer to an array of at least swapchainCount element; this
        // parameter is optional and can be set to null, but if we provide such
        // an array, the result of the presenting operation will be stored in
        // each of its elements, for each swap chain respectively; a single
        // value returned by the whole function is the same as the worst result
        // value from all swap chains.
        presentInfo.pResults = nullptr; // Optional

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        // Although many drivers and platforms trigger VK_ERROR_OUT_OF_DATE_KHR
        // automatically after a window resize, it is not guaranteed to happen.
        // That's why we'll add some extra code to also handle resizes
        // explicitly.
        // It is important to do this after vkQueuePresentKHR to ensure that the
        // semaphores are in a consistent state, otherwise a signaled semaphore
        // may never be properly waited upon.
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            framebufferResized)
        {
            info("Present: Recreating swapchain");
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void CreateSyncObjects()
    {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        // VkSemaphoreCreateInfo semaphoreInfo{};
        // semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        //
        // VkFenceCreateInfo fenceInfo{};
        // fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (!createVkSemaphore(device, imageAvailableSemaphores[i]) ||
                !createVkSemaphore(device, renderFinishedSemaphores[i]) ||
                !createVkFence(device, true, inFlightFences[i]))
            {
                throw std::runtime_error(
                    "failed to create synchronization objects for a frame!");
            }

            // if (vkCreateSemaphore(
            //         device,
            //         &semaphoreInfo,
            //         nullptr,
            //         &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            //     vkCreateSemaphore(
            //         device,
            //         &semaphoreInfo,
            //         nullptr,
            //         &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            //     vkCreateFence(
            //         device,
            //         &fenceInfo,
            //         nullptr,
            //         &inFlightFences[i]) != VK_SUCCESS)
            // {
            //     throw std::runtime_error(
            //         "failed to create synchronization objects for a frame!");
            // }
        }
    }

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            fatal("Failed to begin recording command buffer");
            assert(false);
            throw std::runtime_error(
                "failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        // The last two parameters define the clear values to use for
        // VK_ATTACHMENT_LOAD_OP_CLEAR, which we used as load operation for the
        // color attachment. I've defined the clear color to simply be black
        // with 100% opacity.
        // VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        // renderPassInfo.clearValueCount = 1;
        // renderPassInfo.pClearValues = &clearColor;
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount =
            static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(
            commandBuffer,
            &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(
            commandBuffer,
            indexBuffer,
            0,
            VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &descriptorSets[currentFrame],
            0,
            nullptr);

        // vkCmdDraw(
        //     commandBuffer,
        //     static_cast<uint32_t>(vertices.size()),
        //     1,
        //     0,
        //     0);

        vkCmdDrawIndexed(
            commandBuffer,
            static_cast<uint32_t>(indices.size()),
            1,
            0,
            0,
            0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CreateCommandBuffers()
    {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (std::uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(
                device,
                &allocInfo,
                commandBuffers.data()) != VK_SUCCESS)
        {
            fatal("Failed to allocate command buffers");
            assert(false);
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void CreateCommandPool()
    {
        QueueFamilies queueFamilyIndices =
            getQueueFamilies(physicalDevice, surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
            VK_SUCCESS)
        {
            fatal("Failed to create command pool");
            assert(false);
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void CreateFramebuffers()
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            std::array<VkImageView, 3> attachments = {
                colorImageView,
                depthImageView,
                swapChainImageViews[i],
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount =
                static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                    device,
                    &framebufferInfo,
                    nullptr,
                    &swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        // The format of the color attachment should match the format of the
        // swap chain images, and we're not doing anything with multisampling
        // yet, so we'll stick to 1 sample.
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = msaaSamples;
        // The loadOp and storeOp determine what to do with the data in the
        // attachment before rendering and after rendering. In our case we're
        // going to use the clear operation to clear the framebuffer to black
        // before drawing a new frame. We're interested in seeing the rendered
        // triangle on the screen, so we're going with the store operation here.
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // he loadOp and storeOp apply to color and depth data, and
        // stencilLoadOp / stencilStoreOp apply to stencil data. Our application
        // won't do anything with the stencil buffer, so the results of loading
        // and storing are irrelevant.
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        // Textures and framebuffers in Vulkan are represented by VkImage
        // objects with a certain pixel format, however the layout of the pixels
        // in memory can change based on what you're trying to do with an image.
        // images need to be transitioned to specific layouts that are suitable
        // for the operation that they're going to be involved in next. The
        // initialLayout specifies which layout the image will have before the
        // render pass begins. The finalLayout specifies the layout to
        // automatically transition to when the render pass finishes. Using
        // VK_IMAGE_LAYOUT_UNDEFINED for initialLayout means that we don't care
        // what previous layout the image was in. The caveat of this special
        // value is that the contents of the image are not guaranteed to be
        // preserved, but that doesn't matter since we're going to clear it
        // anyway. We want the image to be ready for presentation using the swap
        // chain after rendering, which is why we use
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR as finalLayout.
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // That's because multisampled images cannot be presented directly. We
        // first need to resolve them to a regular image. This requirement does
        // not apply to the depth buffer, since it won't be presented at any
        // point. Therefore we will have to add only one new attachment for
        // color which is a so-called resolve attachment:
        colorAttachment.finalLayout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Every subpass references one or more of the attachments that we've
        // described using the structure in the previous sections. The
        // attachment parameter specifies which attachment to reference by its
        // index in the attachment descriptions array. Our array consists of a
        // single VkAttachmentDescription, so its index is 0. The layout
        // specifies which layout we would like the attachment to have during a
        // subpass that uses this reference. Vulkan will automatically
        // transition the attachment to this layout when the subpass is started.
        // We intend to use the attachment to function as a color buffer and the
        // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout will give us the best
        // performance, as its name implies.
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // A single render pass can consist of multiple subpasses. Subpasses are
        // subsequent rendering operations that depend on the contents of
        // framebuffers in previous passes, for example a sequence of
        // post-processing effects that are applied one after another. If you
        // group these rendering operations into one render pass, then Vulkan is
        // able to reorder the operations and conserve memory bandwidth for
        // possibly better performance. For our very first triangle, however,
        // we'll stick to a single subpass.
        VkSubpassDescription subpass{};
        // Vulkan may also support compute subpasses in the future, so we have
        // to be explicit about this being a graphics subpass.
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments = {
            colorAttachment,
            depthAttachment,
            colorAttachmentResolve,
        };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        // renderPassInfo.attachmentCount = 1;
        renderPassInfo.attachmentCount =
            static_cast<uint32_t>(attachments.size());
        // The VkAttachmentReference objects reference attachments using the
        // indices of this array.
        // renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
            VK_SUCCESS)
        {
            fatal("Failed to create render pass");
            assert(false);
            // throw std::runtime_error("failed to create render pass!");
        }
    }

    void CreateSwapChain()
    {
        SurfaceDetails swapChainSupport =
            getSurfaceDetails(physicalDevice, surface);

        createVkSwapchainKHR(
            swapChainSupport,
            physicalDevice,
            surface,
            device,
            window,
            swapChain);

        VkSurfaceFormatKHR surfaceFormat =
            selectSurfaceFormat(swapChainSupport.formats);
        // VkPresentModeKHR presentMode =
        //     chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = selectExtent(swapChainSupport.capabilities, window);

        std::uint32_t imageCount =
            swapChainSupport.capabilities.minImageCount + 1;

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(
            device,
            swapChain,
            &imageCount,
            swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;

        swapChainImageViews.resize(swapChainImages.size());
        for (std::uint32_t i{ 0 }; i < swapChainImages.size(); ++i)
        {
            createVkImageView(
                device,
                swapChainImages[i],
                swapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
                swapChainImageViews[i]);
        }
        // createVulkanSwapchain(context.device, surface, window,
        // context.swapchain);
        //
        // swapChain = context.swapchain.handle;
        // swapChainImageFormat = context.swapchain.imageFormat;
        // swapChainExtent = context.swapchain.extent;
        // swapChainImages = context.swapchain.swapChainImages;
        // swapChainImageViews = context.swapchain.swapChainImageViews;
    }

    void CreateLogicaDevice()
    {
        VulkanDevice vulkanDevice;
        createVulkanDevice(instance, surface, vulkanDevice);

        physicalDevice = vulkanDevice.physicalDevice;
        device = vulkanDevice.logicalDevice;
        graphicsQueue = vulkanDevice.graphicsQueue;
        presentQueue = vulkanDevice.presentQueue;
        msaaSamples = vulkanDevice.msaaSamples;

        // VulkanSwapchain vulkanSwapchain;
        // createVulkanSwapchain(vulkanDevice, surface, window,
        // vulkanSwapchain);
        //
        // swapChain = vulkanSwapchain.handle;
        // swapChainExtent = vulkanSwapchain.extent;
        // swapChainImageFormat = vulkanSwapchain.imageFormat;
        // swapChainImages = vulkanSwapchain.swapChainImages;
        // swapChainImageViews = vulkanSwapchain.swapChainImageViews;

        // selectVkPhysicalDevice(instance, surface, physicalDevice);
        //
        // QueueFamilies indices = getQueueFamilies(physicalDevice, surface);
        //
        // createVkDevice(physicalDevice, indices, device);
        //
        // vkGetDeviceQueue(
        //     device,
        //     indices.graphicsFamily.value(),
        //     0,
        //     &graphicsQueue);
        // vkGetDeviceQueue(
        //     device,
        //     indices.presentFamily.value(),
        //     0,
        //     &presentQueue);
    }
};
}
