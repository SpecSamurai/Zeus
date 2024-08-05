#pragma once

#include "Camera.hpp"
#include "Device.hpp"
#include "DeviceBuilder.hpp"
#include "Instance.hpp"
#include "InstanceBuilder.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "SwapchainBuilder.hpp"
#include "Vertex.hpp"
#include "files.hpp"
#include "glfw_utils.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_descriptors.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_image.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_sync.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>
#include <math/definitions.hpp>
#include <math/transformations.hpp>
#include <math/trigonometric.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <array>
#include <chrono>
#include <vector>

namespace Zeus
{
const std::string MODEL_PATH = "D:/Code/Zeus/textures/viking_room.obj";
const std::string TEXTURE_PATH = "D:/Code/Zeus/textures/viking_room.png";

const std::vector<Vertex> vertices{
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },

    { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },

    { { -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },

    { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },

    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },

    { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } }
};

// const std::vector<uint16_t> indices = {
//     0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4
// };

// const std::vector<Vertex> vertices = {
//     { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
//     { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
//     { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
//     { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
// };

// const std::vector<Vertex> vertices = {
//     { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // top-left
//     { { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },  // top-right
//     { { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // botto-right
//     { { -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f } } // bottom-left
// };

const std::vector<std::uint16_t> indices{ 0, 1, 2, 2, 3, 0 };

class VulkanContext
{
    float lastX = 800 / 2.f;
    float lastY = 600 / 2.f;
    bool firstMouse = true;

    Camera camera;

public:
    VulkanContext()
        : camera{ Camera(Vector3f(0.f, 0.f, 2.f), Vector3f(0.f, 0.f, 0.f)) }
    {
    }

    int ref = 1;
    float x = 0.f, y = 0.f, z = 0.f;
    // float x = 2.f, y = 2.f, z = 2.f;

    void updateUniformBuffer(uint32_t currentImage)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        UniformBufferObject ubo{};

        // ubo.model = translation(Vector3f(0.1f, 0.2f, 0.3f));
        // ubo.model = scale({ 1.f, 0.1f, 1.0f });
        ubo.model = identity<float>();
        ubo.view = identity<float>();
        ubo.proj = identity<float>();
        // ubo.view = glm::identity<glm::mat4>(); // identity();
        // switch (ref)
        // {
        // case 1:
        //     ubo.model = reflectionOrigin();
        //     break;
        // case 2:
        //     ubo.model = reflectionX();
        //     break;
        // case 3:
        //     ubo.model = reflectionY();
        //     break;
        // case 4:
        //     ubo.model = rotation(time * radians(90.f), Vector3f(0, 0, 1));
        // ubo.model = translation(Vector3f(x, y, z));
        // ubo.view = camera.GetViewMatrix2();
        //     break;
        // default:
        //     ubo.model = identity();
        ubo.model = rotation(time * radians(90.f), Vector3f(0, 1, 0));
        // ubo.model = translation(Vector3f(x, y, z));
        // ubo.model = translation(Vector3f(x, y, z));
        //     break;
        // }

        // glm::length(glm::vec3());
        // glm::normalize(glm::vec4(1));
        // glm::dot(glm::vec2(1));

        float aspect = swapchain.extent.width / (float)swapchain.extent.height;

        // ubo.proj = orthographic(
        //     -5,
        //     5,
        //     //(float)swapchain.extent.width,
        //     -5,
        //     // (float)swapchain.extent.height,
        //     5,
        //     0.1f,
        //     2.f);

        ubo.view = camera.GetViewMatrix();
        // ubo.view = lookAt(
        //     Vector3f(x, y, z),
        //     Vector3f(0.0f, 0.0f, 0.0f),
        //     Vector3f(0.0f, 1.0f, 0.0f));

        ubo.proj = perspective<float>(radians(45.f), aspect, 0.1f, 10.0f);

        // ubo.model = glm::rotate( glm::mat4(1.0f),
        //     time * glm::radians(90.0f),
        //     glm::vec3(0.0f, 0.0f, 1.0f));
        // ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        // // uno.view = glm::identity<glm::mat4>();
        // ubo.view = glm::lookAt(
        //     glm::vec3(0.0f, 0.0f, 2.0f),
        //     glm::vec3(0.0f, 0.0f, 0.0f),
        //     glm::vec3(0.0f, 1.0f, 0.0f));
        // ubo.proj = glm::perspective(
        //     glm::radians(45.0f),
        //     swapchain.extent.width / (float)swapchain.extent.height,
        //     0.0f,
        //     10.0f);
        // ubo.proj[1][1] *= -1;

        // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
        // glm::vec3(0.0f, 0.0f, 1.0f)); ubo.model =
        // glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)); ubo.view =
        // glm::identity<glm::mat4>(); ubo.view =
        // glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        // glm::vec3(0.0f, 0.0f, 1.0f)); ubo.proj =
        // glm::perspective(glm::radians(45.0f), swapchain.extent.width /
        // (float) swapchain.extent.height, 0.1f, 10.0f); ubo.proj =
        // glm::ortho(-1, 1, -1, 1, -1, 1); ubo.proj[1][1] *= -1;

        // All of the transformations are defined now, so we can copy the data
        // in the uniform buffer object to the current uniform buffer. This
        // happens in exactly the same way as we did for vertex buffers, except
        // without a staging buffer. As noted earlier, we only map the uniform
        // buffer once, so we can directly write to it without having to map
        // again:

        // Using a UBO this way is not the most efficient way to pass frequently
        // changing values to the shader. A more efficient way to pass a small
        // buffer of data to shaders are push constants.
        memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void Init(int width, int height, const char* title)
    {
        // Window
        window = createGlfwWindow(width, height, title);

        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, glfwKeyCallback);
        glfwSetCursorPosCallback(window, glfwCursorPositionCallback);
        glfwSetScrollCallback(window, glfwScrollCallback);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        // Instance
        InstanceBuilder instanceBuilder;
        instanceBuilder.setAppName(INSTANCE_DEFAULT.APPLICATION_NAME)
            .setApplicationVersion(INSTANCE_DEFAULT.APPLICATION_VERSION)
            .setEngineName(INSTANCE_DEFAULT.ENGINE_NAME)
            .setEngineVersion(INSTANCE_DEFAULT.APPLICATION_VERSION)
            .setExtensions(getRequiredGlobalExtensions());

        instance = instanceBuilder.build().value();

        // Device
        createVkSurfaceKHR(instance.handle, window, surface);

        PhysicalDeviceSelector selector(instance);
        selector.setSurface(surface).requirePresent();
        auto physicalDevice = selector.select();

        DeviceBuilder deviceBuilder(physicalDevice.value());
        device = deviceBuilder.build().value();

        // Swapchain
        swapchainBuilder = SwapchainBuilder{};
        swapchainBuilder.setDevice(device, surface)
            .setDesiredPresentMode(
                VSYNC ? VK_PRESENT_MODE_MAILBOX_KHR
                      : VK_PRESENT_MODE_IMMEDIATE_KHR)
            .addDesiredSurfaceFormat({
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
            })
            .addDesiredSurfaceFormat(INSTANCE_DEFAULT.SURFACE_FORMAT)
            .setDesiredExtent(
                static_cast<std::uint32_t>(width),
                static_cast<std::uint32_t>(height));

        swapchain = swapchainBuilder.build().value();

        // createVulkanSwapchain(device, surface, window, swapchain);

        RenderPassConfig renderPassConfig{
            .colorAttachmentFormat = swapchain.imageFormat,
            .depthAttachmentFormat = findDepthFormat(),
            .msaaSamples = VK_SAMPLE_COUNT_1_BIT,
        };

        createVkRenderPass(device.logicalDevice, renderPassConfig, renderPass);

        // We're now able to pass arbitrary attributes to the vertex shader for
        // each vertex, but what about global variables? We're going to move on
        // to 3D graphics from this chapter on and that requires a
        // model-view-projection matrix. We could include it as vertex data, but
        // that's a waste of memory and it would require us to update the vertex
        // buffer whenever the transformation changes. The transformation could
        // easily change every single frame.

        // The right way to tackle this in Vulkan is to use resource
        // descriptors. A descriptor is a way for shaders to freely access
        // resources like buffers and images. We're going to set up a buffer
        // that contains the transformation matrices and have the vertex shader
        // access them through a descriptor. Usage of descriptors consists of
        // three parts:

        // Specify a descriptor layout during pipeline creation
        // Allocate a descriptor set from a descriptor pool
        // Bind the descriptor set during rendering
        // The descriptor layout specifies the types of resources that are going
        // to be accessed by the pipeline, just like a render pass specifies the
        // types of attachments that will be accessed. A descriptor set
        // specifies the actual buffer or image resources that will be bound to
        // the descriptors, just like a framebuffer specifies the actual image
        // views to bind to render pass attachments. The descriptor set is then
        // bound for the drawing commands just like the vertex buffers and
        // framebuffer.
        createDescriptorSetLayout(device.logicalDevice, descriptorSetLayout);

        auto vertShaderCode = readFile(
            "D:/Code/Zeus/build/debug/src/editor/shaders/shader.vert.spv");
        auto fragShaderCode = readFile(
            "D:/Code/Zeus/build/debug/src/editor/shaders/shader.frag.spv");
        GraphicsPipelineConfig config{
            .device = device.logicalDevice,
            .renderPass = renderPass,
            .descriptorSetLayout = descriptorSetLayout,
            .msaaSamples = VK_SAMPLE_COUNT_1_BIT, // msaaSamples,
            .bindingDescription = getBindingDescription(),
            .attributeDescriptions = getAttributeDescriptions(),
            .vertShaderCode = vertShaderCode,
            .fragShaderCode = fragShaderCode,
        };

        createGraphicsVkPipeline(config, pipelineLayout, graphicsPipeline);

        // QueueFamilies queueFamilyIndices =
        //     getQueueFamilies(physicalDevice, surface);

        createVkCommandPool(
            device.logicalDevice,
            device.graphicsFamily,
            commandPool);

        // createColorResources();
        createDepthResources();

        // moved becouase of depth buffer
        // void CreateFramebuffers()
        swapchainFramebuffers.resize(swapchain.imageViews.size());

        for (std::size_t i{ 0 }; i < swapchain.imageViews.size(); ++i)
        {
            std::vector<VkImageView> attachments = {
                swapchain.imageViews[i],
                depthImageView,
                // colorImageView,
            };

            createVkFramebuffer(
                device.logicalDevice,
                attachments,
                renderPass,
                swapchain.extent,
                swapchainFramebuffers[i]);
        }

        // createTextureImage();
        //
        // // createTextureImageView();
        // createVkImageView(
        //     device,
        //     textureImage,
        //     VK_FORMAT_R8G8B8A8_SRGB,
        //     VK_IMAGE_ASPECT_COLOR_BIT,
        //     mipLevels,
        //     textureImageView);
        //
        // createTextureSampler();
        // loadModel();

        createVertexBuffer();
        createIndexBuffer();

        createUniformBuffers();
        createDescriptorPool(device.logicalDevice, descriptorPool);
        createDescriptorSets();

        commandBuffers.resize(FRAMES_IN_FLIGHT);
        allocateVkCommandBuffers(
            device.logicalDevice,
            commandPool,
            commandBuffers);

        imageAvailableSemaphores.resize(FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
        inFlightFences.resize(FRAMES_IN_FLIGHT);

        for (std::size_t i{ 0 }; i < FRAMES_IN_FLIGHT; ++i)
        {
            if (!createVkSemaphore(
                    device.logicalDevice,
                    imageAvailableSemaphores[i]) ||
                !createVkSemaphore(
                    device.logicalDevice,
                    renderFinishedSemaphores[i]) ||
                !createVkFence(device.logicalDevice, true, inFlightFences[i]))
            {
                throw std::runtime_error(
                    "failed to create synchronization objects for a frame!");
            }
        }
    }

    void Destroy()
    {
        info("Terminating VulkanContext");

        DestroySwapchain();

        vkDestroyPipeline(device.logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device.logicalDevice, pipelineLayout, nullptr);

        vkDestroyRenderPass(device.logicalDevice, renderPass, nullptr);

        for (std::size_t i{ 0 }; i < FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroyBuffer(device.logicalDevice, uniformBuffers[i], nullptr);
            vkFreeMemory(
                device.logicalDevice,
                uniformBuffersMemory[i],
                nullptr);
        }

        vkDestroyDescriptorPool(device.logicalDevice, descriptorPool, nullptr);

        // vkDestroySampler(device, textureSampler, nullptr);
        // vkDestroyImageView(device, textureImageView, nullptr);

        // vkDestroyImage(device, textureImage, nullptr);
        // vkFreeMemory(device, textureImageMemory, nullptr);

        vkDestroyDescriptorSetLayout(
            device.logicalDevice,
            descriptorSetLayout,
            nullptr);

        vkDestroyBuffer(device.logicalDevice, indexBuffer, nullptr);
        vkFreeMemory(device.logicalDevice, indexBufferMemory, nullptr);

        vkDestroyBuffer(device.logicalDevice, vertexBuffer, nullptr);
        vkFreeMemory(device.logicalDevice, vertexBufferMemory, nullptr);

        for (std::size_t i{ 0 }; i < FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroySemaphore(
                device.logicalDevice,
                renderFinishedSemaphores[i],
                nullptr);
            vkDestroySemaphore(
                device.logicalDevice,
                imageAvailableSemaphores[i],
                nullptr);
            vkDestroyFence(device.logicalDevice, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device.logicalDevice, commandPool, nullptr);

        destroyDevice(device);

        vkDestroySurfaceKHR(instance.handle, surface, nullptr);

        destroyInstance(instance);

        info("Terminating window");
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    // That's all it takes to recreate the swap chain! However, the disadvantage
    // of this approach is that we need to stop all rendering before creating
    // the new swap chain. It is possible to create a new swap chain while
    // drawing commands on an image from the old swap chain are still in-flight.
    // You need to pass the previous swap chain to the oldSwapChain field in the
    // VkSwapchainCreateInfoKHR struct and destroy the old swap chain as soon as
    // you've finished using it.
    void RecreateSwapchain()
    {
        // WIDTH and HEIGH are set in GLFW screen coordinates
        // which might not corresponde to pixel coordinates due to higher
        // pixel density
        // in this case Vulkan didnt fixed the swap extend for us
        // int width, height;
        // to query the resolution of the window in pixel before matching it
        // against the minimum and maximum image extent
        // glfwGetFramebufferSize(window, &width, &height);

        // Handles minimazation
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        // Should recreate the renderpass. It's possible for the swap chain
        // image format to change when moving a window from LDR to HDR monitor.
        vkDeviceWaitIdle(device.logicalDevice);

        const auto& result = swapchainBuilder.setOldSwapchain(swapchain.handle)
                                 .setDesiredExtent(
                                     static_cast<std::uint32_t>(width),
                                     static_cast<std::uint32_t>(height))
                                 .build();
        DestroySwapchain();
        swapchain = result.value();

        // createColorResources();
        createDepthResources();

        swapchainFramebuffers.resize(swapchain.imageViews.size());

        for (std::size_t i{ 0 }; i < swapchain.imageViews.size(); ++i)
        {
            std::vector<VkImageView> attachments = {
                swapchain.imageViews[i],
                // colorImageView,
                depthImageView,
            };

            createVkFramebuffer(
                device.logicalDevice,
                attachments,
                renderPass,
                swapchain.extent,
                swapchainFramebuffers[i]);
        }
    }

    void DestroySwapchain()
    {
        vkDestroyImageView(device.logicalDevice, depthImageView, nullptr);
        vkDestroyImage(device.logicalDevice, depthImage, nullptr);
        vkFreeMemory(device.logicalDevice, depthImageMemory, nullptr);

        // vkDestroyImageView(device, colorImageView, nullptr);
        // vkDestroyImage(device, colorImage, nullptr);
        // vkFreeMemory(device, colorImageMemory, nullptr);

        for (auto framebuffer : swapchainFramebuffers)
        {
            vkDestroyFramebuffer(device.logicalDevice, framebuffer, nullptr);
        }

        destroySwapchain(device, swapchain);
    }

    void createVertexBuffer()
    {
        // The flags parameter is used to configure sparse buffer memory, which
        // is not relevant right now. We'll leave it at the default value of 0.
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        BufferConfig stageBufferConfig{
            .size = bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createVkBuffer(
            device.logicalDevice,
            device.physicalDevice,
            stageBufferConfig,
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
        vkMapMemory(
            device.logicalDevice,
            stagingBufferMemory,
            0,
            bufferSize,
            0,
            &data);

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
        memcpy(data, vertices.data(), static_cast<std::size_t>(bufferSize));
        vkUnmapMemory(device.logicalDevice, stagingBufferMemory);

        // The vertex buffer we have right now works correctly, but the memory
        // type that allows us to access it from the CPU may not be the most
        // optimal memory type for the graphics card itself to read from. The
        // most optimal memory has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag
        // and is usually not accessible by the CPU on dedicated graphics cards.

        BufferConfig vertexBufferConfig{
            .size = bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };

        createVkBuffer(
            device.logicalDevice,
            device.physicalDevice,
            vertexBufferConfig,
            vertexBuffer,
            vertexBufferMemory);

        cmdCopyBuffer(
            device,
            commandPool,
            stagingBuffer,
            vertexBuffer,
            bufferSize);

        vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(device.logicalDevice, stagingBufferMemory, nullptr);
    }

    void createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        BufferConfig stageBufferConfig{
            .size = bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createVkBuffer(
            device.logicalDevice,
            device.physicalDevice,
            stageBufferConfig,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(
            device.logicalDevice,
            stagingBufferMemory,
            0,
            bufferSize,
            0,
            &data);

        memcpy(data, indices.data(), static_cast<std::size_t>(bufferSize));

        // When we define all memory ranges that should be flashed, we can call
        // the vkFlushMappedMemoryRanges() function. After that, the driver will
        // know which parts were modified and will reload them (that is, refresh
        // cache). Reloading usually occurs on barriers. After modifying a
        // buffer, we should set a buffer memory barrier, which will tell the
        // driver that some operations influenced a buffer and it should be
        // refreshed. But, fortunately, in this case such a barrier is placed
        // implicitly by the driver on a submission of a command buffer that
        // references the given buffer and no additional operations are
        // required. Now we can use this buffer during rendering commands
        // recording.

        // VkMappedMemoryRange flush_range = {
        //     VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, // VkStructureType sType
        //     nullptr,                    // const void            *pNext
        //     Vulkan.VertexBuffer.Memory, // VkDeviceMemory         memory
        //     0,                          // VkDeviceSize           offset
        //     VK_WHOLE_SIZE               // VkDeviceSize           size
        // };
        // vkFlushMappedMemoryRanges(GetDevice(), 1, &flush_range);

        vkUnmapMemory(device.logicalDevice, stagingBufferMemory);

        BufferConfig indexBufferConfig{
            .size = bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };

        createVkBuffer(
            device.logicalDevice,
            device.physicalDevice,
            indexBufferConfig,
            indexBuffer,
            indexBufferMemory);

        cmdCopyBuffer(
            device,
            commandPool,
            stagingBuffer,
            indexBuffer,
            bufferSize);

        vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(device.logicalDevice, stagingBufferMemory, nullptr);
    }

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        beginVkCommandBuffer(commandBuffer);

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };

        cmdBeginVkRenderPass(
            renderPass,
            swapchain.extent,
            swapchainFramebuffers[imageIndex],
            clearValues,
            commandBuffer);

        vkCmdBindPipeline(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipeline);

        cmdSetVkViewport(swapchain.extent, commandBuffer);
        cmdSetVkScissor(swapchain.extent, commandBuffer);

        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        // The previous chapter already mentioned that you should allocate
        // multiple resources like buffers from a single memory allocation, but
        // in fact you should go a step further. Driver developers recommend
        // that you also store multiple buffers, like the vertex and index
        // buffer, into a single VkBuffer and use offsets in commands like
        // vkCmdBindVertexBuffers. The advantage is that your data is more cache
        // friendly in that case, because it's closer together. It is even
        // possible to reuse the same chunk of memory for multiple resources if
        // they are not used during the same render operations, provided that
        // their data is refreshed, of course. This is known as aliasing and
        // some Vulkan functions have explicit flags to specify that you want to
        // do this.
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        // Using an index buffer for drawing involves two changes to
        // recordCommandBuffer. We first need to bind the index buffer, just
        // like we did for the vertex buffer. The difference is that you can
        // only have a single index buffer. It's unfortunately not possible to
        // use different indices for each vertex attribute, so we do still have
        // to completely duplicate vertex data even if just one attribute
        // varies.
        vkCmdBindIndexBuffer(
            commandBuffer,
            indexBuffer,
            0,
            VK_INDEX_TYPE_UINT16);
        // VK_INDEX_TYPE_UINT32);

        // Unlike vertex and index buffers, descriptor sets are not unique to
        // graphics pipelines. Therefore we need to specify if we want to bind
        // descriptor sets to the graphics or compute pipeline. The next
        // parameter is the layout that the descriptors are based on. The next
        // three parameters specify the index of the first descriptor set, the
        // number of sets to bind, and the array of sets to bind. We'll get back
        // to this in a moment. The last two parameters specify an array of
        // offsets that are used for dynamic descriptors. We'll look at these in
        // a future chapter.
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &descriptorSets[currentFrame],
            0,
            nullptr);

        // vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdDraw(
            commandBuffer,
            static_cast<uint32_t>(vertices.size()),
            1,
            0,
            0);

        // vkCmdDrawIndexed(
        //     commandBuffer,
        //     static_cast<uint32_t>(indices.size()),
        //     1,
        //     0,
        //     0,
        //     0);

        vkCmdEndRenderPass(commandBuffer);

        endVkCommandBuffer(commandBuffer);
    }

    void DrawFrame()
    {
        vkWaitForFences(
            device.logicalDevice,
            1,
            &inFlightFences[currentFrame],
            VK_TRUE,
            UINT64_MAX);

        std::uint32_t imageIndex;
        VkResult acquireNextImageResult{
            vkAcquireNextImageKHR(
                device.logicalDevice,
                swapchain.handle,
                UINT64_MAX,
                imageAvailableSemaphores[currentFrame],
                VK_NULL_HANDLE,
                &imageIndex),
        };

        // switch (result)
        // {
        // case VK_SUCCESS:
        // case VK_SUBOPTIMAL_KHR:
        //     break;
        // case VK_ERROR_OUT_OF_DATE_KHR:
        //     return OnWindowSizeChanged();
        // default:
        //     std::cout << "Problem occurred during swap chain image
        //     acquisition!"
        //               << std::endl;
        //     return false;
        // }
        // You could also decide to do that if the swap chain is suboptimal, but
        // I've chosen to proceed anyway in that case because we've already
        // acquired an image.
        if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            info(
                "AcquireNextImage: Recreating swapchain. {}",
                vkResultToString(acquireNextImageResult));

            RecreateSwapchain();
            return;
        }
        else if (
            acquireNextImageResult != VK_SUCCESS &&
            acquireNextImageResult != VK_SUBOPTIMAL_KHR)
        {
            error(
                "Failed to acquire swap chain image. {}",
                vkResultToString(acquireNextImageResult));

            return;
        }

        updateUniformBuffer(currentFrame);

        // Delay resetting the fence until after we know for sure we will be
        // submitting work with it. Thus, if we return early, the fence is still
        // signaled and vkWaitForFences wont deadlock the next time we use the
        // same fence object.
        vkResetFences(device.logicalDevice, 1, &inFlightFences[currentFrame]);

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

        VkResult submitResult{
            vkQueueSubmit(
                device.graphicsQueue,
                1,
                &submitInfo,
                inFlightFences[currentFrame]),
        };

        if (submitResult != VK_SUCCESS)
        {
            error(
                "Failed to submit draw command buffer. {}",
                vkResultToString(submitResult));

            return;
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapchain.handle };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        // A pointer to an array of at least swapchainCount element; this
        // parameter is optional and can be set to null, but if we provide such
        // an array, the result of the presenting operation will be stored in
        // each of its elements, for each swap chain respectively; a single
        // value returned by the whole function is the same as the worst result
        // value from all swap chains.
        presentInfo.pResults = nullptr;

        VkResult presentResult{
            vkQueuePresentKHR(device.presentQueue, &presentInfo)
        };

        // Although many drivers and platforms trigger VK_ERROR_OUT_OF_DATE_KHR
        // automatically after a window resize, it is not guaranteed to happen.
        // That's why we'll add some extra code to also handle resizes
        // explicitly.
        // It is important to do this after vkQueuePresentKHR to ensure that the
        // semaphores are in a consistent state, otherwise a signaled semaphore
        // may never be properly waited upon.
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
            presentResult == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            info(
                "Present: Recreating swapchain. {}",
                vkResultToString(presentResult));

            framebufferResized = false;
            RecreateSwapchain();
        }
        else if (presentResult != VK_SUCCESS)
        {
            error(
                "Failed to present swap chain image. {}",
                vkResultToString(presentResult));

            return;
        }

        currentFrame = (currentFrame + 1) % FRAMES_IN_FLIGHT;
    }

    // In the next chapter we'll specify the buffer that contains the UBO data
    // for the shader, but we need to create this buffer first. We're going to
    // copy new data to the uniform buffer every frame, so it doesn't really
    // make any sense to have a staging buffer. It would just add extra overhead
    // in this case and likely degrade performance instead of improving it.

    // We should have multiple buffers, because multiple frames may be in flight
    // at the same time and we don't want to update the buffer in preparation of
    // the next frame while a previous one is still reading from it! Thus, we
    // need to have as many uniform buffers as we have frames in flight, and
    // write to a uniform buffer that is not currently being read by the GPU

    // We map the buffer right after creation using vkMapMemory to get a pointer
    // to which we can write the data later on. The buffer stays mapped to this
    // pointer for the application's whole lifetime. This technique is called
    // "persistent mapping" and works on all Vulkan implementations. Not having
    // to map the buffer every time we need to update it increases performances,
    // as mapping is not free.
    void createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(FRAMES_IN_FLIGHT);

        for (std::size_t i{ 0 }; i < FRAMES_IN_FLIGHT; ++i)
        {
            BufferConfig config{
                .size = bufferSize,
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            };

            createVkBuffer(
                device.logicalDevice,
                device.physicalDevice,
                config,
                uniformBuffers[i],
                uniformBuffersMemory[i]);

            vkMapMemory(
                device.logicalDevice,
                uniformBuffersMemory[i],
                0,
                bufferSize,
                0,
                &uniformBuffersMapped[i]);
        }
    }

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

        // We must specify a location (index) for each attribute (the same as in
        // a shader source code, in location layout qualifier), source of data
        // (binding from which data will be read), format (data type and number
        // of components), and offset at which data for this specific attribute
        // can be found (offset from the beginning of a data for a given vertex,
        // not from the beginning of all vertex data

        // location – Index of an attribute, the same as defined by the location
        // layout specifier in a shader source code.
        // binding – The number of the slot from which data should be read
        // (source of data like VBO in OpenGL), the same binding as in a
        // VkVertexInputBindingDescription structure and
        // vkCmdBindVertexBuffers() function (described later). format – Data
        // type and number of components per attribute. offset – Beginning of
        // data for a given attribute.
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        // attributeDescriptions[2].binding = 0;
        // attributeDescriptions[2].location = 2;
        // attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        // attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    void createDescriptorSets()
    {
        // In our case we will create one descriptor set for each frame in
        // flight, all with the same layout. Unfortunately we do need all the
        // copies of the layout because the next function expects an array
        // matching the number of sets.
        std::vector<VkDescriptorSetLayout> layouts(
            FRAMES_IN_FLIGHT,
            descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(FRAMES_IN_FLIGHT);
        // The call to vkAllocateDescriptorSets will allocate descriptor sets,
        // each with one uniform buffer descriptor.
        if (vkAllocateDescriptorSets(
                device.logicalDevice,
                &allocInfo,
                descriptorSets.data()) != VK_SUCCESS)
        {
            return;
            // throw std::runtime_error("failed to allocate descriptor sets!");
        }

        // The descriptor sets have been allocated now, but the descriptors
        // within still need to be configured. We'll now add a loop to populate
        // every descriptor:
        for (std::size_t i{ 0 }; i < FRAMES_IN_FLIGHT; ++i)
        {
            // Descriptors that refer to buffers, like our uniform buffer
            // descriptor, are configured with a VkDescriptorBufferInfo struct.
            // This structure specifies the buffer and the region within it that
            // contains the data for the descriptor.
            // If you're overwriting the whole buffer, like we are in this case,
            // then it is also possible to use the VK_WHOLE_SIZE value for the
            // range. The configuration of descriptors is updated using the
            // vkUpdateDescriptorSets function, which takes an array of
            // VkWriteDescriptorSet structs as parameter.
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            // VkDescriptorImageInfo imageInfo{};
            // imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // imageInfo.imageView = textureImageView;
            // imageInfo.sampler = textureSampler;

            // The first two fields specify the descriptor set to update and the
            // binding. We gave our uniform buffer binding index 0. Remember
            // that descriptors can be arrays, so we also need to specify the
            // first index in the array that we want to update. We're not using
            // an array, so the index is simply 0.
            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType =
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            // descriptorWrite.pImageInfo = nullptr; // Optional
            // descriptorWrite.pTexelBufferView = nullptr; // Optional

            // The last field references an array with descriptorCount structs
            // that actually configure the descriptors. It depends on the type
            // of descriptor which one of the three you actually need to use.
            // The pBufferInfo field is used for descriptors that refer to
            // buffer data, pImageInfo is used for descriptors that refer to
            // image data, and pTexelBufferView is used for descriptors that
            // refer to buffer views.

            // descriptorWrites[1].sType =
            // VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // descriptorWrites[1].dstSet = descriptorSets[i];
            // descriptorWrites[1].dstBinding = 1;
            // descriptorWrites[1].dstArrayElement = 0;
            // descriptorWrites[1].descriptorType =
            //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            // descriptorWrites[1].descriptorCount = 1;
            // descriptorWrites[1].pImageInfo = &imageInfo;

            // The updates are applied using vkUpdateDescriptorSets. It accepts
            // two kinds of arrays as parameters: an array of
            // VkWriteDescriptorSet and an array of VkCopyDescriptorSet. The
            // latter can be used to copy descriptors to each other, as its name
            // implies.
            vkUpdateDescriptorSets(
                device.logicalDevice,
                static_cast<uint32_t>(descriptorWrites.size()),
                descriptorWrites.data(),
                0,
                nullptr);
        }
    }

    void createDepthResources()
    {
        VkFormat depthFormat = findDepthFormat();

        ImageConfig config{
            .width = swapchain.extent.width,
            .height = swapchain.extent.height,
            .mipLevels = 1,
            .numSamples = VK_SAMPLE_COUNT_1_BIT, // msaaSamples,
            .format = depthFormat,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };

        createVkImage(
            device.logicalDevice,
            device.physicalDevice,
            config,
            depthImage,
            depthImageMemory);

        createVkImageView(
            device.logicalDevice,
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
        //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        //     1);
    }

    VkFormat findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(
                device.physicalDevice,
                format,
                &props);

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

    void transitionImageLayout(
        VkImage image,
        [[maybe_unused]] VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        uint32_t mipLevels)
    {
        // VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

        // vkCmdPipelineBarrier(
        //     commandBuffer,
        //     sourceStage,
        //     destinationStage,
        //     0,
        //     0,
        //     nullptr,
        //     0,
        //     nullptr,
        //     1,
        //     &barrier);

        // endSingleTimeCommands(commandBuffer);
    }

    void createColorResources()
    {
        VkFormat colorFormat = swapchain.imageFormat;

        // We're also using only one mip level, since this is enforced by the
        // Vulkan specification in case of images with more than one sample per
        // pixel. Also, this color buffer doesn't need mipmaps since it's not
        // going to be used as a texture:
        // createImage(
        //     swapchain.extent.width,
        //     swapchain.extent.height,
        //     1,
        //     VK_SAMPLE_COUNT_1_BIT, // msaaSamples,
        //     colorFormat,
        //     VK_IMAGE_TILING_OPTIMAL,
        //     VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
        //         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        //     colorImage,
        //     colorImageMemory);

        createVkImageView(
            device.logicalDevice,
            colorImage,
            colorFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            colorImageView);
    }

    void createTextureSampler()
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device.physicalDevice, &properties);

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
        samplerInfo.maxLod = 1; // static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(
                device.logicalDevice,
                &samplerInfo,
                nullptr,
                &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
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
        // mipLevels = static_cast<uint32_t>(
        //                 std::floor(std::log2(std::max(texWidth, texHeight))))
        //                 +
        //             1;

        if (!pixels)
        {
            fatal("Failed to load texture image");
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        // CreateBuffer(
        //     {},
        //     imageSize,
        //     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        //         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        //     stagingBuffer,
        //     stagingBufferMemory);

        void* data;
        vkMapMemory(
            device.logicalDevice,
            stagingBufferMemory,
            0,
            imageSize,
            0,
            &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device.logicalDevice, stagingBufferMemory);

        stbi_image_free(pixels);

        // createImage(
        //     (std::uint32_t)texWidth,
        //     (std::uint32_t)texHeight,
        //     1, // mipLevels,
        //     VK_SAMPLE_COUNT_1_BIT,
        //     VK_FORMAT_R8G8B8A8_SRGB,
        //     VK_IMAGE_TILING_OPTIMAL,
        //     VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
        //     |
        //         VK_IMAGE_USAGE_SAMPLED_BIT,
        //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        //     textureImage,
        //     textureImageMemory);

        transitionImageLayout(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1 // mipLevels
        );

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

        vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(device.logicalDevice, stagingBufferMemory, nullptr);

        // generateMipmaps(
        //     textureImage,
        //     VK_FORMAT_R8G8B8A8_SRGB,
        //     texWidth,
        //     texHeight,
        //     mipLevels);
    }

    void copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height)
    {
        // VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

        // vkCmdCopyBufferToImage(
        //     commandBuffer,
        //     buffer,
        //     image,
        //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        //     1,
        //     &region);

        // endSingleTimeCommands(commandBuffer);
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
            device.physicalDevice,
            imageFormat,
            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures &
              VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error(
                "texture image format does not support linear blitting!");
        }

        // VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

            // vkCmdPipelineBarrier(
            //     commandBuffer,
            //     VK_PIPELINE_STAGE_TRANSFER_BIT,
            //     VK_PIPELINE_STAGE_TRANSFER_BIT,
            //     0,
            //     0,
            //     nullptr,
            //     0,
            //     nullptr,
            //     1,
            //     &barrier);

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

            // vkCmdBlitImage(
            //     commandBuffer,
            //     image,
            //     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            //     image,
            //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            //     1,
            //     &blit,
            //     VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            // vkCmdPipelineBarrier(
            //     commandBuffer,
            //     VK_PIPELINE_STAGE_TRANSFER_BIT,
            //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            //     0,
            //     0,
            //     nullptr,
            //     0,
            //     nullptr,
            //     1,
            //     &barrier);

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

        // vkCmdPipelineBarrier(
        //     commandBuffer,
        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
        //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        //     0,
        //     0,
        //     nullptr,
        //     0,
        //     nullptr,
        //     1,
        //     &barrier);

        // endSingleTimeCommands(commandBuffer);
    }

private:
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

    static void framebufferResizeCallback(
        GLFWwindow* window,
        int width,
        int height);

public:
    GLFWwindow* window{ nullptr };

    Instance instance{};

    VkSurfaceKHR surface{ VK_NULL_HANDLE };

    Device device{};

    VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };

    SwapchainBuilder swapchainBuilder;
    Swapchain swapchain{};

    // VulkanSwapchain swapchain{};

    // intel
    // We are doing this in order to simplify the code called in a rendering
    // loop. In a normal, real-life scenario we wouldn’t (probably) create a
    // framebuffer for each swap chain image. I assume that a better solution
    // would be to render into a single image (texture) and after that use
    // command buffers that would copy rendering results from that image into a
    // given swap chain image. This way we will have only three simple command
    // buffers that are connected with a swap chain. All other rendering
    // commands would be independent of a swap chain, making it easier to
    // maintain.

    // We've talked a lot about framebuffers in the past few chapters and we've
    // set up the render pass to expect a single framebuffer with the same
    // format as the swap chain images, but we haven't actually created any yet.

    // The attachments specified during render pass creation are bound by
    // wrapping them into a VkFramebuffer object. A framebuffer object
    // references all of the VkImageView objects that represent the attachments.
    // In our case that will be only a single one: the color attachment.
    // However, the image that we have to use for the attachment depends on
    // which image the swap chain returns when we retrieve one for presentation.
    // That means that we have to create a framebuffer for all of the images in
    // the swap chain and use the one that corresponds to the retrieved image at
    // drawing time.
    std::vector<VkFramebuffer> swapchainFramebuffers{};

    VkRenderPass renderPass{ VK_NULL_HANDLE };
    VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
    VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
    VkPipeline graphicsPipeline{ VK_NULL_HANDLE };

    VkCommandPool commandPool{ VK_NULL_HANDLE };

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

    // uint32_t mipLevels;
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

    // std::vector<Vertex> vertices;
    // std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers{};

    // Each frame should have its own command buffer, set of semaphores, and
    // fence. Rename and then change them to be std::vectors of the objects
    std::vector<VkSemaphore> imageAvailableSemaphores{};
    std::vector<VkSemaphore> renderFinishedSemaphores{};
    std::vector<VkFence> inFlightFences{};
    std::uint32_t currentFrame{ 0 };
    bool framebufferResized{ false };
};
}
