#pragma once

#define GLFW_INCLUDE_VULKAN

#include "files.hpp"
#include "graphics/VulkanDevice.cpp"
#include "graphics/VulkanInstance.hpp"
#include "graphics/glfw_utils.hpp"
#include "graphics/vulkan_settings.hpp"
#include "graphics/vulkan_utils.hpp"
#include "logger.hpp"

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>

// TODO: EX https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
// TRANSFER QUEUE

namespace Zeus
{
struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2>
    getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2>
            attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

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

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

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

        vkDestroyDevice(device, nullptr);

        if (ENABLE_VALIDATION_LAYERS)
        {
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        trace("Terminating window");

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Run(int width, int height, const char* title)
    {
        _title = title;

        // window.InitWindow(width, height, title);

        window = createWindow(width, height, title);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        InitVulkan();
        MainLoop();
    }

private:
    const char* _title;

    GLFWwindow* window;

    VkInstance instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    // VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VulkanDevice physical;
    VkDevice device{VK_NULL_HANDLE};

    VkQueue graphicsQueue{VK_NULL_HANDLE};
    VkQueue presentQueue{VK_NULL_HANDLE};

    VkSwapchainKHR swapChain{VK_NULL_HANDLE};
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
        CreateInstance(instance, _title);
        SetupDebugMessenger(instance, debugMessenger);
        // m_instance.Init(_title);

        CreateSurface();
        physical.PickPhysicalDevice(instance, surface);

        // PickPhysicalDevice();
        CreateLogicaDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        createDescriptorSetLayout();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    void createDescriptorPool()
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // Not fully correct! maxSets parameter specifies how many descriptor
        // sets can be allocated from a given pool. But poolSizes parameter
        // specifies the number of elements in the pPoolSizes array. And this
        // array describes how many descriptors of a certain type will be
        // allocated NOT in a single descriptor set but in total from a given
        // pool. poolSize.descriptorCount parameter specifies the number of
        // descriptors of a given type which can be allocated in total from a
        // given pool (across all sets).
        poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        // But again - through the pPoolSizes array You specify the total number
        // of descriptors that can be allocated from a pool (not the total
        // number of descriptors in a set or the total number of sets).
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(
                device,
                &poolInfo,
                nullptr,
                &descriptorPool) != VK_SUCCESS)
        {
            critical("failed to create descriptor pool!");
            throw std::runtime_error("failed to create descriptor pool!");
        }
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

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;       // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
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

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

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

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
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
        vkGetPhysicalDeviceMemoryProperties(
            physical.physicalDevice,
            &memProperties);

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
        for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
        {
            vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
        }

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(device, swapChainImageViews[i], nullptr);
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

        CreateSwapChain();
        CreateImageViews();
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

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {
            renderFinishedSemaphores[currentFrame]};
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

        VkSwapchainKHR swapChains[] = {swapChain};
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

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(
                    device,
                    &semaphoreInfo,
                    nullptr,
                    &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(
                    device,
                    &semaphoreInfo,
                    nullptr,
                    &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(
                    device,
                    &fenceInfo,
                    nullptr,
                    &inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "failed to create synchronization objects for a frame!");
            }
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
            critical("Failed to begin recording command buffer");
            assert(false);
            throw std::runtime_error(
                "failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        // The last two parameters define the clear values to use for
        // VK_ATTACHMENT_LOAD_OP_CLEAR, which we used as load operation for the
        // color attachment. I've defined the clear color to simply be black
        // with 100% opacity.
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

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
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(
            commandBuffer,
            indexBuffer,
            0,
            VK_INDEX_TYPE_UINT16);

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
            critical("Failed to allocate command buffers");
            assert(false);
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices =
            physical.FindQueueFamilies(physical.physicalDevice, surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
            VK_SUCCESS)
        {
            critical("Failed to create command pool");
            assert(false);
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void CreateFramebuffers()
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                    device,
                    &framebufferInfo,
                    nullptr,
                    &swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                critical("Failed to create framebuffer");
                assert(false);
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void CreateGraphicsPipeline()
    {
        // Shader modules are just a thin wrapper around the shader bytecode
        // that we've previously loaded from a file and the functions defined in
        // it. The compilation and linking of the SPIR-V bytecode to machine
        // code for execution by the GPU doesn't happen until the graphics
        // pipeline is created. That means that we're allowed to destroy the
        // shader modules again as soon as pipeline creation is finished, which
        // is why we'll make them local variables in the createGraphicsPipeline
        // function instead of class members:
        auto vertShaderCode = readFile("shaders/shader.vert.spv");
        auto fragShaderCode = readFile("shaders/shader.frag.spv");

        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        // The next two members specify the shader module containing the code,
        // and the function to invoke, known as the entrypoint. That means that
        // it's possible to combine multiple fragment shaders into a single
        // shader module and use different entry points to differentiate between
        // their behaviors. In this case we'll stick to the standard main,
        // however.
        vertShaderStageInfo.pName = "main";

        // There is one more (optional) member, pSpecializationInfo, which we
        // won't be using here, but is worth discussing. It allows you to
        // specify values for shader constants. You can use a single shader
        // module where its behavior can be configured at pipeline creation by
        // specifying different values for the constants used in it. This is
        // more efficient than configuring the shader using variables at render
        // time, because the compiler can do optimizations like eliminating if
        // statements that depend on these values. If you don't have any
        // constants like that, then you can set the member to nullptr, which
        // our struct initialization does automatically.
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertShaderStageInfo,
            fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions =
            attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        // rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f;          // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;          // Optional
        multisampling.pSampleMask = nullptr;            // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE;      // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor =
            VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor =
            VK_BLEND_FACTOR_ZERO;                            // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor =
            VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor =
            VK_BLEND_FACTOR_ZERO;                            // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount =
            static_cast<std::uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;                 // Optional
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;         // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr;      // Optional

        if (vkCreatePipelineLayout(
                device,
                &pipelineLayoutInfo,
                nullptr,
                &pipelineLayout) != VK_SUCCESS)
        {
            critical("Failed to create pipeline layout");
            assert(false);
            // throw std::runtime_error("failed to create pipeline layout!");
        }

        // VkViewport viewport{};
        // viewport.x = 0.0f;
        // viewport.y = 0.0f;
        // viewport.width = (float)swapChainExtent.width;
        // viewport.height = (float)swapChainExtent.height;
        // viewport.minDepth = 0.0f;
        // viewport.maxDepth = 1.0f;
        //
        // VkRect2D scissor{};
        // scissor.offset = {0, 0};
        // scissor.extent = swapChainExtent;
        // VkPipelineViewportStateCreateInfo viewportState{};
        // viewportState.sType =
        //     VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        // viewportState.viewportCount = 1;
        // viewportState.pViewports = &viewport;
        // viewportState.scissorCount = 1;
        // viewportState.pScissors = &scissor;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1;              // Optional

        if (vkCreateGraphicsPipelines(
                device,
                VK_NULL_HANDLE,
                1,
                &pipelineInfo,
                nullptr,
                &graphicsPipeline) != VK_SUCCESS)
        {
            critical("Failed to create graphics pipeline");
            assert(false);

            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    VkShaderModule CreateShaderModule(const std::vector<char>& code)
    {
        // The one catch is that the size of the bytecode is specified in bytes,
        // but the bytecode pointer is a uint32_t pointer rather than a char
        // pointer. Therefore we will need to cast the pointer with
        // reinterpret_cast as shown below. When you perform a cast like this,
        // you also need to ensure that the data satisfies the alignment
        // requirements of uint32_t. Lucky for us, the data is stored in an
        // std::vector where the default allocator already ensures that the data
        // satisfies the worst case alignment requirements.
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
            VK_SUCCESS)
        {
            critical("Failed to create shader module.");
            assert(false);
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    void CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        // The format of the color attachment should match the format of the
        // swap chain images, and we're not doing anything with multisampling
        // yet, so we'll stick to 1 sample.
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
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
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        // The VkAttachmentReference objects reference attachments using the
        // indices of this array.
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
            VK_SUCCESS)
        {
            critical("Failed to create render pass");
            assert(false);
            // throw std::runtime_error("failed to create render pass!");
        }
    }

    void CreateImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (std::size_t i = 0; i < swapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // It indicates that the image is used as a color attachment or a
            // source or destination for copying or blitting color data.
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;

            // If you were working on a stereographic 3D application, then you
            // would create a swap chain with multiple layers. You could then
            // create multiple image views for each image representing the views
            // for the left and right eyes by accessing different layers.
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(
                    device,
                    &createInfo,
                    nullptr,
                    &swapChainImageViews[i]) != VK_SUCCESS)
            {
                critical("Failed to create image views");
                assert(false);
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport =
            physical.querySwapChainSupport(physical.physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat =
            chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode =
            chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        // simply sticking to this minimum means that we may sometimes have to
        // wait on the driver to complete internal operations before we can
        // acquire another image to render to. Therefore it is recommended to
        // request at least one more image than the minimum
        // One more image may be useful for triple buffering-like presentation
        // mode
        std::uint32_t imageCount =
            swapChainSupport.capabilities.minImageCount + 1;
        // 0 is a special value that means that there is no maximum
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        // we can't use format from one pair and colorspace from another pair
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        // Defines the number of layers in a swap chain images (that is, views);
        // typically this value will be one but if we want to create multiview
        // or stereo (stereoscopic 3D) images, we can set it to some higher
        // value.
        createInfo.imageArrayLayers = 1; // always 1 unless you are developing a
                                         // stereoscopic 3D application
        // The imageUsage bit field specifies what kind of operations we'll use
        // the images in the swap chain for. In this tutorial we're going to
        // render directly to them, which means that they're used as color
        // attachment. It is also possible that you'll render images to a
        // separate image first to perform operations like post-processing. In
        // that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT
        // instead and use a memory operation to transfer the rendered image to
        // a swap chain image.
        // Usage flags define how a given image may be used in Vulkan. If we
        // want an image to be sampled (used inside shaders) it must be created
        // with “sampled” usage. If the image should be used as a depth render
        // target, it must be created with “depth and stencil” usage. An image
        // without proper usage “enabled” cannot be used for a given purpose or
        // the results of such operations will be undefined.

        // For a swap chain we want to render (in most cases) into the image
        // (use it as a render target), so we must specify “color attachment”
        // usage with VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT enum. In Vulkan this
        // usage is always available for swap chains, so we can always set it
        // without any additional checking. But for any other usage we must
        // ensure it is supported – we can do this through a
        // “supportedUsageFlags” member of surface capabilities structure.
        // “transfer destination” usage which is required for image clear
        // operation.
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices =
            physical.FindQueueFamilies(physical.physicalDevice, surface);
        std::uint32_t queueFamilyIndices[] = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()};

        // Next, we need to specify how to handle swap chain images that will be
        // used across multiple queue families. That will be the case in our
        // application if the graphics queue family is different from the
        // presentation queue. We'll be drawing on the images in the swap chain
        // from the graphics queue and then submitting them on the presentation
        // queue. There are two ways to handle images that are accessed from
        // multiple queues:
        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;     // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }
        // We can specify that a certain transform should be applied to images
        // in the swap chain if it is supported (supportedTransforms in
        // capabilities), like a 90 degree clockwise rotation or horizontal
        // flip. To specify that you do not want any transformation, simply
        // specify the current transformation.
        // On some platforms we may want our image to be transformed. This is
        // usually the case on tablets when they are oriented in a way other
        // than their default orientation. During swap chain creation we must
        // specify what transformations should be applied to images prior to
        // presenting. We can, of course, use only the supported transforms,
        // which can be found in a “supportedTransforms” member of acquired
        // surface capabilities.

        // If the selected pre-transform is other than the current
        // transformation (also found in surface capabilities) the presentation
        // engine will apply the selected transformation. On some platforms this
        // may cause performance degradation (probably not noticeable but worth
        // mentioning). In the sample code, I don't want any transformations
        // but, of course, I must check whether it is supported. If not, I'm
        // just using the same transformation that is currently used.
        // Here we don't want any transformations to occur so if the identity
        // transform is supported use it
        // otherwise just use the same transform as current transform
        createInfo.preTransform =
            swapChainSupport.capabilities.currentTransform;

        // The compositeAlpha field specifies if the alpha channel should be
        // used for blending with other windows in the window system. You'll
        // almost always want to simply ignore the alpha channel, hence
        // VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR. When this flag is set, the alpha
        // channel of the presented image is treated as opaque (fully opaque),
        // meaning that the image is considered to completely cover the contents
        // of the underlying surface.
        // This mode is useful when the application knows that the rendered
        // image completely fills the surface and does not require alpha
        // blending with the contents beneath it.
        // This parameter is used to indicate how the surface (image) should be
        // composited (blended?) with other surfaces on some windowing systems;
        // this value must also be one of the possible values (bits) returned in
        // surface capabilities, but it looks like opaque composition (no
        // blending, alpha ignored) will be always supported (as most of the
        // games will want to use this mode).
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;

        // If the clipped member is set to VK_TRUE then that means that we don't
        // care about the color of pixels that are obscured, for example because
        // another window is in front of them. Unless you really need to be able
        // to read these pixels back and get predictable results, you'll get the
        // best performance by enabling clipping.
        createInfo.clipped = VK_TRUE;

        // auto oldSwapChain = swapChain;
        createInfo.oldSwapchain = VK_NULL_HANDLE; // oldSwapChain

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
            VK_SUCCESS)
        {
            critical("Failed to create swap chain");
            assert(false);
            throw std::runtime_error("failed to create swap chain!");
        }

        // if (oldSwapchain != VK_NULL_HANDLE)
        // {
        //     vkDestroySwapchainKHR(device, oldSwapChain, nullptr);
        // }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(
            device,
            swapChain,
            &imageCount,
            swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void CreateLogicaDevice()
    {
        QueueFamilyIndices indices =
            physical.FindQueueFamilies(physical.physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<std::uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
        };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount =
            static_cast<std::uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount =
            static_cast<std::uint32_t>(DEVICE_EXTENSIONS.size());
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        // Previous implementations of Vulkan made a distinction between
        // instance and device specific validation layers, but this is no longer
        // the case. That means that the enabledLayerCount and
        // ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by
        // up-to-date implementations. However, it is still a good idea to set
        // them anyway to be compatible with older implementations:
        if (ENABLE_VALIDATION_LAYERS)
        {
            createInfo.enabledLayerCount =
                static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(
                physical.physicalDevice,
                &createInfo,
                nullptr,
                &device) != VK_SUCCESS)
        {
            critical("Failed to create logical device");
            assert(false);
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(
            device,
            indices.graphicsFamily.value(),
            0,
            &graphicsQueue);
        vkGetDeviceQueue(
            device,
            indices.presentFamily.value(),
            0,
            &presentQueue);
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            // Choosing a format for the images depends on the type of
            // processing/rendering we want to do, that is, if we want to blend
            // the application window with the desktop contents, an alpha value
            // may be required. We must also know what color space is available
            // and if we operate on linear or sRGB colorspace.
            // Each platform may support a different number of format-colorspace
            // pairsEach platform may support a different number of
            // format-colorspace pairh
            // // If the list contains only one entry with undefined format
            // it means that there are no preferred surface formats and any can
            // be chosen
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        // return VK_PRESENT_MODE_IMMEDIATE_KHR;
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        // when this is true then vulkan tells us(by setting currentExten)
        // that we have to use this(match the resolution of the window)
        // *** Special value of surface extent is width == height == -1
        // If this is so we define the size by ourselves but it must fit within
        // defined confines
        if (capabilities.currentExtent.width !=
            std::numeric_limits<std::uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        // if false then it tells us that this window manager allow us to differ
        else
        {
            // WIDTH and HEIGH are set in GLFW screen coordinates
            // which might not corresponde to pixel coordinates due to higher
            // pixel density
            // in this case Vulkan didnt fixed the swap extend for us
            int width, height;
            // to query the resolution of the window in pixel before matching it
            // against the minimum and maximum image extent
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<std::uint32_t>(width),
                static_cast<std::uint32_t>(height)};

            actualExtent.width = std::clamp(
                actualExtent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(
                actualExtent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void CreateSurface()
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
            VK_SUCCESS)
        {
            critical("Failed to create window surface");
            throw std::runtime_error("failed to create window surface!");
        }
    }
};
}
