#include "Device.hpp"

#include "PhysicalDeviceSelector.hpp"
#include "VkContext.hpp"
#include "rhi/vulkan_command.hpp"
#include "rhi/vulkan_debug.hpp"
#include "rhi/vulkan_memory.hpp"
#include "rhi/vulkan_sync.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <functional>
#include <set>

namespace Zeus
{
void Device::Init(VkInstance instance, VkSurfaceKHR surface)
{
    VkPhysicalDeviceFeatures requestedFeatures{};
    requestedFeatures.sampleRateShading = VK_TRUE;
    requestedFeatures.samplerAnisotropy = VK_TRUE;
    requestedFeatures.fillModeNonSolid = VK_TRUE;

    VkPhysicalDeviceVulkan13Features features1_3{};
    features1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features1_3.dynamicRendering = VK_TRUE;
    features1_3.synchronization2 = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features1_2{};
    features1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features1_2.pNext = &features1_3;
    features1_2.bufferDeviceAddress = VK_TRUE;
    features1_2.descriptorIndexing = VK_TRUE;
    features1_2.descriptorBindingPartiallyBound = VK_TRUE;
    features1_2.descriptorBindingVariableDescriptorCount = VK_TRUE;
    features1_2.runtimeDescriptorArray = VK_TRUE;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features1_2;
    features2.features = requestedFeatures;

    std::vector<const char*> extensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        // VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        // VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        // VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    };

    PhysicalDeviceSelectorInfo info{
        .extensions = extensions,
        .instance = instance,
        .surface = surface,

        .preferredType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,

        .features2 = features2,
        .features = requestedFeatures,
        .features1_2 = features1_2,
        .features1_3 = features1_3,

        .requirePresent = true,
        .dedicatedTransferQueue = true,
        .dedicatedComputeQueue = true,
    };

    auto selectedDevice{ PhysicalDeviceSelector::Select(info) };
    if (!selectedDevice.has_value())
    {
        assert(false && "Physical Device not found.");
    }

    PhysicalDevice physicalDevice{ selectedDevice.value() };

    const auto& limits{ physicalDevice.properties.limits };
    m_maxImageDimension2D = limits.maxImageDimension2D;
    m_maxPushConstantsSize = limits.maxPushConstantsSize;

    m_physicalDevice = physicalDevice.handle;
    m_graphicsFamily = physicalDevice.queueFamilies.graphicsFamily.value();
    m_presentFamily = physicalDevice.queueFamilies.presentFamily.value();
    m_transferFamily = physicalDevice.queueFamilies.transferFamily.value();
    m_computeFamily = physicalDevice.queueFamilies.computeFamily.value();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<std::uint32_t> uniqueQueueFamilies{
        m_graphicsFamily,
        m_presentFamily,
        m_transferFamily,
        m_computeFamily,
    };

    float queuePriority{ 1.0f };
    for (std::uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount =
        static_cast<std::uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount =
        static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    // createInfo.pEnabledFeatures = &physicalDevice.features;
    createInfo.pNext = &features2;

#ifndef NDEBUG
    // Deprecated but set for backwards compatibility
    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

    VKCHECK(
        vkCreateDevice(
            physicalDevice.handle,
            &createInfo,
            allocationCallbacks.get(),
            &m_logicalDevice),
        "Failed to create logical device.");

    vkGetDeviceQueue(m_logicalDevice, m_graphicsFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, m_presentFamily, 0, &m_presentQueue);
    vkGetDeviceQueue(m_logicalDevice, m_transferFamily, 0, &m_transferQueue);
    vkGetDeviceQueue(m_logicalDevice, m_computeFamily, 0, &m_computeQueue);

    m_deletionQueue.Init(m_logicalDevice);

    createVkCommandPool(
        VkContext::GetLogicalDevice(),
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        VkContext::GetDevice().GetQueueFamily(QueueType::Transfer),
        &m_ImmediateSubmitCommandPool);

    allocateVkCommandBuffer(
        &m_ImmediateSubmitCommandBuffer,
        VkContext::GetLogicalDevice(),
        m_ImmediateSubmitCommandPool);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        VkContext::GetLogicalDevice(),
        VK_OBJECT_TYPE_COMMAND_POOL,
        reinterpret_cast<std::uint64_t>(m_ImmediateSubmitCommandPool),
        "CommandPool Immediate");

    setDebugUtilsObjectNameEXT(
        VkContext::GetLogicalDevice(),
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        reinterpret_cast<std::uint64_t>(m_ImmediateSubmitCommandBuffer),
        "CommandBuffer Immediate");
#endif

    VKCHECK(
        createVkFence(
            VkContext::GetLogicalDevice(),
            true,
            &m_ImmediateSubmitFence),
        "Failed to create fence.");

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        VkContext::GetLogicalDevice(),
        VK_OBJECT_TYPE_FENCE,
        reinterpret_cast<std::uint64_t>(m_ImmediateSubmitFence),
        "Fence Immediate Submit");
#endif
}

void Device::Destroy()
{
    vkDestroyCommandPool(
        VkContext::GetLogicalDevice(),
        m_ImmediateSubmitCommandPool,
        allocationCallbacks.get());

    vkDestroyFence(
        VkContext::GetLogicalDevice(),
        m_ImmediateSubmitFence,
        allocationCallbacks.get());

    vkDestroyDevice(m_logicalDevice, allocationCallbacks.get());

    m_ImmediateSubmitCommandPool = VK_NULL_HANDLE;
    m_ImmediateSubmitCommandBuffer = VK_NULL_HANDLE;
    m_ImmediateSubmitFence = VK_NULL_HANDLE;

    m_graphicsQueue = VK_NULL_HANDLE;
    m_presentQueue = VK_NULL_HANDLE;
    m_transferQueue = VK_NULL_HANDLE;
    m_computeQueue = VK_NULL_HANDLE;

    m_logicalDevice = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
}

void Device::Wait()
{
    vkDeviceWaitIdle(m_logicalDevice);
}

void Device::WaitAll()
{
    // for (uint32_t i = 0; i < 2; i++)
    // {
    //     queues::regular[i]->Wait();
    // }
}

void Device::CmdImmediateSubmit(
    std::function<void(VkCommandBuffer cmd)>&& function)
{
    VKCHECK(
        vkResetFences(
            VkContext::GetLogicalDevice(),
            1,
            &m_ImmediateSubmitFence),
        "Failed to reset fence");

    VKCHECK(
        vkResetCommandBuffer(m_ImmediateSubmitCommandBuffer, 0),
        "Failed to reset command buffer");

    beginVkCommandBuffer(
        m_ImmediateSubmitCommandBuffer,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    function(m_ImmediateSubmitCommandBuffer);

    VKCHECK(
        vkEndCommandBuffer(m_ImmediateSubmitCommandBuffer),
        "Failed to end command buffer");

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        m_ImmediateSubmitCommandBuffer) };

    cmdVkQueueSubmit2(
        VkContext::GetDevice().GetQueue(QueueType::Transfer),
        m_ImmediateSubmitFence,
        0,
        nullptr,
        1,
        &submitInfo,
        0,
        nullptr);

    VKCHECK(
        vkWaitForFences(
            VkContext::GetLogicalDevice(),
            1,
            &m_ImmediateSubmitFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");
}

VkQueue Device::GetQueue(QueueType type) const
{
    switch (type)
    {
    case QueueType::Graphics:
        return m_graphicsQueue;
    case QueueType::Present:
        return m_presentQueue;
    case QueueType::Transfer:
        return m_transferQueue;
    case QueueType::Compute:
        return m_computeQueue;
    default:
        assert(false && "Queue type not supported");
    }
}

std::uint32_t Device::GetQueueFamily(QueueType type) const
{
    switch (type)
    {
    case QueueType::Graphics:
        return m_graphicsFamily;
    case QueueType::Present:
        return m_presentFamily;
    case QueueType::Transfer:
        return m_transferFamily;
    case QueueType::Compute:
        return m_computeFamily;
    default:
        assert(false && "Queue type not supported");
    }
}

VkDevice Device::GetLogicalDevice() const
{
    return m_logicalDevice;
}

VkPhysicalDevice Device::GetPhysicalDevice() const
{
    return m_physicalDevice;
}

DeletionQueue& Device::GetDeletionQueue()
{
    return m_deletionQueue;
}

std::uint32_t Device::GetMaxImageDimension2D() const
{
    return m_maxImageDimension2D;
}

std::uint32_t Device::GetMaxPushConstantsSize() const
{
    return m_maxPushConstantsSize;
}
}
