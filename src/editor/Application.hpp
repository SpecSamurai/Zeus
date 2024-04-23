#pragma once

#define GLFW_INCLUDE_VULKAN

#include "files.hpp"
#include "graphics/VulkanDevice.cpp"
#include "graphics/VulkanInstance.cpp"
#include "graphics/Window.cpp"
#include "logger.hpp"
#include "vulkan.utils.hpp"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

namespace Zeus
{
// static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//     VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//     [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
//     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//     [[maybe_unused]] void* pUserData)
// {
//     switch (messageSeverity)
//     {
//     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
//     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
//         info("{}", pCallbackData->pMessage);
//         break;
//     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
//         warning("{}", pCallbackData->pMessage);
//         break;
//     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
//         error("{}", pCallbackData->pMessage);
//         break;
//     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
//         break;
//     }
//
//     return VK_FALSE;
// }

// struct QueueFamilyIndices
// {
//     std::optional<std::uint32_t> graphicsFamily;
//     std::optional<std::uint32_t> presentFamily;
//
//     bool isComplete()
//     {
//         return graphicsFamily.has_value() && presentFamily.has_value();
//     }
// };
//
// struct SwapChainSupportDetails
// {
//     VkSurfaceCapabilitiesKHR capabilities;
//     std::vector<VkSurfaceFormatKHR> formats;
//     std::vector<VkPresentModeKHR> presentModes;
// };

// const std::vector<const char*> validationLayers = {
//     "VK_LAYER_KHRONOS_validation",
// };

// const std::vector<const char*> deviceExtensions = {
//     VK_KHR_SWAPCHAIN_EXTENSION_NAME,
// };

// #ifdef NDEBUG
// const bool ENABLE_VALIDATION_LAYERS = false;
// #else
// const bool ENABLE_VALIDATION_LAYERS = true;
// #endif

class Application
{
public:
    ~Application()
    {
        info("Terminating application");

        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
        vkDestroyFence(device, inFlightFence, nullptr);

        vkDestroyCommandPool(device, commandPool, nullptr);

        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);

        vkDestroyDevice(device, nullptr);

        // Instance
        if (ENABLE_VALIDATION_LAYERS)
        {
            destroyDebugUtilsMessengerEXT(
                m_instance.instance,
                m_instance.debugMessenger,
                nullptr);
        }

        vkDestroySurfaceKHR(m_instance.instance, surface, nullptr);
        vkDestroyInstance(m_instance.instance, nullptr);
        // Instance

        // glfwDestroyWindow(window);
        // glfwTerminate();
    }

    void Run(int width, int height, const char* title)
    {
        _title = title;

        window.InitWindow(width, height, title);

        InitVulkan();
        MainLoop();
    }

private:
    const char* _title;

    Window window;
    VulkanInstance m_instance;

    // VkInstance instance{VK_NULL_HANDLE};
    // VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    // VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VulkanDevice physical;
    VkDevice device{VK_NULL_HANDLE};

    VkQueue graphicsQueue{VK_NULL_HANDLE};
    VkQueue presentQueue{VK_NULL_HANDLE};

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

private:
    void MainLoop()
    {
        while (!glfwWindowShouldClose(window.m_handle))
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

    void InitVulkan()
    {
        // CreateInstance();
        m_instance.CreateInstance(_title);
        m_instance.SetupDebugMessenger();
        // SetupDebugMessenger();
        CreateSurface();
        physical.PickPhysicalDevice(m_instance.instance, surface);

        // PickPhysicalDevice();
        CreateLogicaDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
    }

    void DrawFrame()
    {
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(
            device,
            swapChain,
            UINT64_MAX,
            imageAvailableSemaphore,
            VK_NULL_HANDLE,
            &imageIndex);

        vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(commandBuffer, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            // VK_PIPELINE_STAGE_TRANSFER_BIT,
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) !=
            VK_SUCCESS)
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

        vkQueuePresentKHR(presentQueue, &presentInfo);
    }

    void CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(
                device,
                &semaphoreInfo,
                nullptr,
                &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(
                device,
                &semaphoreInfo,
                nullptr,
                &renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) !=
                VK_SUCCESS)
        {
            throw std::runtime_error(
                "failed to create synchronization objects for a frame!");
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

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) !=
            VK_SUCCESS)
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
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

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
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
        pipelineLayoutInfo.setLayoutCount = 0;            // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

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

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
            VK_SUCCESS)
        {
            critical("Failed to create swap chain");
            assert(false);
            throw std::runtime_error("failed to create swap chain!");
        }

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
            static_cast<std::uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // Previous implementations of Vulkan made a distinction between
        // instance and device specific validation layers, but this is no longer
        // the case. That means that the enabledLayerCount and
        // ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by
        // up-to-date implementations. However, it is still a good idea to set
        // them anyway to be compatible with older implementations:
        if (ENABLE_VALIDATION_LAYERS)
        {
            createInfo.enabledLayerCount =
                static_cast<std::uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
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

    void CreateSurface()
    {
        if (glfwCreateWindowSurface(
                m_instance.instance,
                window.m_handle,
                nullptr,
                &surface) != VK_SUCCESS)
        {
            critical("Failed to create window surface");
            throw std::runtime_error("failed to create window surface!");
        }
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
            glfwGetFramebufferSize(window.m_handle, &width, &height);

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

    // void PickPhysicalDevice()
    // {
    //     std::uint32_t deviceCount{0};
    //     vkEnumeratePhysicalDevices(m_instance.instance, &deviceCount,
    //     nullptr);
    //
    //     if (deviceCount == 0)
    //     {
    //         throw std::runtime_error(
    //             "failed to find GPUs with Vulkan support!");
    //     }
    //
    //     std::vector<VkPhysicalDevice> devices(deviceCount);
    //     vkEnumeratePhysicalDevices(
    //         m_instance.instance,
    //         &deviceCount,
    //         devices.data());
    //
    //     for (const auto& device : devices)
    //     {
    //         if (IsDeviceSuitable(device))
    //         {
    //             physicalDevice = device;
    //             break;
    //         }
    //     }
    //
    //     if (physicalDevice == VK_NULL_HANDLE)
    //     {
    //         critical("Failed to find a suitable GPU");
    //         assert(false);
    //         throw std::runtime_error("failed to find a suitable GPU!");
    //     }
    // }

    // SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    // {
    //     SwapChainSupportDetails details;
    //
    //     vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    //         device,
    //         surface,
    //         &details.capabilities);
    //
    //     std::uint32_t formatCount;
    //     vkGetPhysicalDeviceSurfaceFormatsKHR(
    //         device,
    //         surface,
    //         &formatCount,
    //         nullptr);
    //
    //     if (formatCount != 0)
    //     {
    //         details.formats.resize(formatCount);
    //         vkGetPhysicalDeviceSurfaceFormatsKHR(
    //             device,
    //             surface,
    //             &formatCount,
    //             details.formats.data());
    //     }
    //
    //     std::uint32_t presentModeCount;
    //     vkGetPhysicalDeviceSurfacePresentModesKHR(
    //         device,
    //         surface,
    //         &presentModeCount,
    //         nullptr);
    //
    //     if (presentModeCount != 0)
    //     {
    //         details.presentModes.resize(presentModeCount);
    //         vkGetPhysicalDeviceSurfacePresentModesKHR(
    //             device,
    //             surface,
    //             &presentModeCount,
    //             details.presentModes.data());
    //     }
    //
    //     return details;
    // }

    // bool IsDeviceSuitable(VkPhysicalDevice device)
    // {
    //     VkPhysicalDeviceProperties deviceProperties;
    //     vkGetPhysicalDeviceProperties(device, &deviceProperties);
    //
    //     VkPhysicalDeviceFeatures deviceFeatures;
    //     vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    //
    //     // return deviceProperties.deviceType ==
    //     //            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //     //        deviceFeatures.geometryShader;
    //
    //     bool isSuitable = // deviceProperties.deviceType ==
    //                       //    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //         deviceFeatures.geometryShader;
    //
    //     if (isSuitable)
    //     {
    //         std::uint32_t major_version =
    //             VK_VERSION_MAJOR(deviceProperties.apiVersion);
    //         std::uint32_t minor_version =
    //             VK_VERSION_MINOR(deviceProperties.apiVersion);
    //         std::int32_t patch_version =
    //             VK_VERSION_PATCH(deviceProperties.apiVersion);
    //         debug("Name: {}", deviceProperties.deviceName);
    //         debug(
    //             "Api Version: {}.{}.{}",
    //             major_version,
    //             minor_version,
    //             patch_version);
    //         // debug("Driver Version: {}", deviceProperties.driverVersion);
    //     }
    //
    //     QueueFamilyIndices indices = FindQueueFamilies(device);
    //
    //     bool extensionsSupported = CheckDeviceExtensionSupport(device);
    //
    //     bool swapChainAdequate = false;
    //     if (extensionsSupported)
    //     {
    //         SwapChainSupportDetails swapChainSupport =
    //             querySwapChainSupport(device);
    //         swapChainAdequate = !swapChainSupport.formats.empty() &&
    //                             !swapChainSupport.presentModes.empty();
    //     }
    //
    //     return isSuitable && indices.isComplete() && extensionsSupported &&
    //            swapChainAdequate;
    // }

    // bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
    // {
    //     std::uint32_t extensionCount;
    //     vkEnumerateDeviceExtensionProperties(
    //         device,
    //         nullptr,
    //         &extensionCount,
    //         nullptr);
    //
    //     std::vector<VkExtensionProperties>
    //     availableExtensions(extensionCount);
    //     vkEnumerateDeviceExtensionProperties(
    //         device,
    //         nullptr,
    //         &extensionCount,
    //         availableExtensions.data());
    //
    //     std::set<std::string> requiredExtensions(
    //         deviceExtensions.begin(),
    //         deviceExtensions.end());
    //
    //     for (const auto& extension : availableExtensions)
    //     {
    //         if (requiredExtensions.contains(extension.extensionName))
    //         {
    //             debug(
    //                 "Device level extension supported: {}",
    //                 extension.extensionName);
    //         }
    //
    //         requiredExtensions.erase(extension.extensionName);
    //     }
    //
    //     return requiredExtensions.empty();
    // }
    //
    // QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    // {
    //     QueueFamilyIndices indices;
    //
    //     std::uint32_t queueFamilyCount = 0;
    //     vkGetPhysicalDeviceQueueFamilyProperties(
    //         device,
    //         &queueFamilyCount,
    //         nullptr);
    //
    //     std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    //     vkGetPhysicalDeviceQueueFamilyProperties(
    //         device,
    //         &queueFamilyCount,
    //         queueFamilies.data());
    //
    //     std::uint32_t i = 0;
    //     for (const auto& queueFamily : queueFamilies)
    //     {
    //         if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    //         {
    //             indices.graphicsFamily = i;
    //         }
    //
    //         VkBool32 presentSupport = false;
    //         vkGetPhysicalDeviceSurfaceSupportKHR(
    //             device,
    //             i,
    //             surface,
    //             &presentSupport);
    //
    //         if (presentSupport)
    //         {
    //             indices.presentFamily = i;
    //         }
    //         if (indices.isComplete())
    //         {
    //             break;
    //         }
    //
    //         i++;
    //     }
    //
    //     return indices;
    // }

    // void CreateInstance()
    // {
    //     if (ENABLE_VALIDATION_LAYERS &&
    //         !checkValidationLayerSupport(validationLayers))
    //     {
    //         critical("Validation layers requested, but not available");
    //         assert(false);
    //     }
    //
    //     VkApplicationInfo appInfo{};
    //     appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    //     appInfo.pApplicationName = _title;
    //     appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    //     appInfo.pEngineName = "Zeus Engine";
    //     appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    //     appInfo.apiVersion = VK_API_VERSION_1_3;
    //
    //     VkInstanceCreateInfo createInfo{};
    //     createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //     createInfo.pApplicationInfo = &appInfo;
    //
    //     auto extensions = getRequiredExtensions();
    //     createInfo.enabledExtensionCount =
    //         static_cast<std::uint32_t>(extensions.size());
    //     createInfo.ppEnabledExtensionNames = extensions.data();
    //
    //     // The debugCreateInfo variable is placed outside the if statement to
    //     // ensure that it is not destroyed before the vkCreateInstance call.
    //     VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    //     if (ENABLE_VALIDATION_LAYERS)
    //     {
    //         createInfo.enabledLayerCount =
    //             static_cast<std::uint32_t>(validationLayers.size());
    //         createInfo.ppEnabledLayerNames = validationLayers.data();
    //
    //         populateDebugMessengerCreateInfo(debugCreateInfo);
    //         createInfo.pNext = &debugCreateInfo;
    //     }
    //     else
    //     {
    //         createInfo.enabledLayerCount = 0;
    //         createInfo.pNext = nullptr;
    //     }
    //
    //     if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    //     {
    //         critical("Failed to create instance");
    //         assert(false);
    //     }
    // }

    // void populateDebugMessengerCreateInfo(
    //     VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    // {
    //     createInfo = {};
    //     createInfo.sType =
    //         VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    //     createInfo.messageSeverity =
    //         VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    //         // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    //         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    //         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    //     createInfo.messageType =
    //         VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    //         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    //         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    //     createInfo.pfnUserCallback = debugCallback;
    // }

    // void SetupDebugMessenger()
    // {
    //     if (!ENABLE_VALIDATION_LAYERS)
    //         return;
    //
    //     VkDebugUtilsMessengerCreateInfoEXT createInfo;
    //     populateDebugMessengerCreateInfo(createInfo);
    //
    //     if (createDebugUtilsMessengerEXT(
    //             m_instance.instance,
    //             &createInfo,
    //             nullptr,
    //             &debugMessenger) != VK_SUCCESS)
    //     {
    //         critical("Failed to set up debug messenger");
    //         assert(false);
    //     }
    // }

    // // get required global extensions
    // std::vector<const char*> getRequiredExtensions()
    // {
    //     std::uint32_t extensionCount = 0;
    //     vkEnumerateInstanceExtensionProperties(
    //         nullptr,
    //         &extensionCount,
    //         nullptr);
    //
    //     std::vector<VkExtensionProperties> Aextensions(extensionCount);
    //     vkEnumerateInstanceExtensionProperties(
    //         nullptr,
    //         &extensionCount,
    //         Aextensions.data());
    //
    //     debug("Supported global extensions:");
    //     for (const auto& extension : Aextensions)
    //     {
    //         debug("> {}", extension.extensionName);
    //     }
    //
    //     std::uint32_t glfwExtensionCount = 0;
    //     const char** glfwExtensions;
    //     glfwExtensions =
    //     glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //
    //     std::vector<const char*> extensions(
    //         glfwExtensions,
    //         glfwExtensions + glfwExtensionCount);
    //
    //     if (ENABLE_VALIDATION_LAYERS)
    //     {
    //         extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    //     }
    //
    //     return extensions;
    // }
};
}
