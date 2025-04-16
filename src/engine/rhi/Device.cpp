#include "Device.hpp"

#include "Definitions.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "VkContext.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <functional>
#include <set>

namespace Zeus
{
void Device::Initialize(VkInstance instance, VkSurfaceKHR surface)
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
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
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
    m_minUniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
    m_minStorageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;

    m_physicalDevice = physicalDevice.handle;
    std::uint32_t graphicsFamily =
        physicalDevice.queueFamilies.graphicsFamily.value();
    std::uint32_t presentFamily =
        physicalDevice.queueFamilies.presentFamily.value();
    std::uint32_t transferFamily =
        physicalDevice.queueFamilies.transferFamily.value();
    std::uint32_t computeFamily =
        physicalDevice.queueFamilies.computeFamily.value();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<std::uint32_t> uniqueQueueFamilies{
        graphicsFamily,
        presentFamily,
        transferFamily,
        computeFamily,
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

    // Initialization below relies on the initialized logical device
    m_graphicsQueue =
        Queue("Queue_Graphics", QueueType::Graphics, graphicsFamily);
    m_presentQueue = Queue("Queue_Present", QueueType::Present, presentFamily);
    m_transferQueue =
        Queue("Queue_Transfer", QueueType::Transfer, transferFamily);
    m_computeQueue = Queue("Queue_Compute", QueueType::Compute, computeFamily);

    m_ImmediateSubmitCommandPool = CommandPool(
        "CommandPool_ImmediateSubmit",
        transferFamily,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    m_ImmediateSubmitCommandBuffer = CommandBuffer(
        m_ImmediateSubmitCommandPool,
        "CommandBuffer_ImmediateSubmit");

    m_ImmediateSubmitFence = Fence("Fence_ImmediateSubmit", true);
}

void Device::Destroy()
{
    m_ImmediateSubmitCommandPool.Destroy();
    m_ImmediateSubmitFence.Destroy();

    vkDestroyDevice(m_logicalDevice, allocationCallbacks.get());

    m_logicalDevice = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
}

void Device::Wait() const
{
    vkDeviceWaitIdle(m_logicalDevice);
}

void Device::WaitAll() const
{
    m_graphicsQueue.Wait();
    m_presentQueue.Wait();
    m_transferQueue.Wait();
    m_computeQueue.Wait();
}

void Device::CmdImmediateSubmit(
    std::function<void(const CommandBuffer& cmd)>&& function)
{
    m_ImmediateSubmitFence.Reset();
    m_ImmediateSubmitCommandBuffer.Reset();

    m_ImmediateSubmitCommandBuffer.Begin(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    function(m_ImmediateSubmitCommandBuffer);

    m_ImmediateSubmitCommandBuffer.End();

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        m_ImmediateSubmitCommandBuffer.GetHandle()) };

    m_transferQueue.Submit(
        m_ImmediateSubmitFence.GetHandle(),
        0,
        nullptr,
        1,
        &submitInfo,
        0,
        nullptr);

    m_ImmediateSubmitFence.Wait();
}

const Queue& Device::GetQueue(QueueType type) const
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

DeviceMemoryBudget Device::GetMemoryBudget() const
{
    DeviceMemoryBudget deviceMemoryBudget{};

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(
        static_cast<VkPhysicalDevice>(m_physicalDevice),
        &memory_properties);

    VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
    vmaGetHeapBudgets(VkContext::Allocator(), budgets);

    for (std::uint32_t i{ 0 }; i < VK_MAX_MEMORY_HEAPS; ++i)
    {
        if (memory_properties.memoryHeaps[i].flags &
            VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            if (budgets[i].budget < 1ull << 60)
            {
                deviceMemoryBudget.budgetBytes += budgets[i].budget;
                deviceMemoryBudget.usageBytes += budgets[i].usage;
            }
        }
    }

    return deviceMemoryBudget;
}

VkDevice Device::GetLogicalDevice() const
{
    return m_logicalDevice;
}

VkPhysicalDevice Device::GetPhysicalDevice() const
{
    return m_physicalDevice;
}

std::uint32_t Device::GetMaxImageDimension2D() const
{
    return m_maxImageDimension2D;
}

std::uint32_t Device::GetMaxPushConstantsSize() const
{
    return m_maxPushConstantsSize;
}

VkDeviceSize Device::GetMinUniformBufferOffsetAlignment() const
{
    return m_minUniformBufferOffsetAlignment;
}

VkDeviceSize Device::GetMinStorageBufferOffsetAlignment() const
{
    return m_minStorageBufferOffsetAlignment;
}
}
