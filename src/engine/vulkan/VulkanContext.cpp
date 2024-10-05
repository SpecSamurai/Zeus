#include "VulkanContext.hpp"

#include "Device.hpp"
#include "DeviceBuilder.hpp"
#include "Instance.hpp"
#include "InstanceBuilder.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "SwapchainBuilder.hpp"
#include "core/logger.hpp"
#include "vulkan_memory.hpp"
#include "vulkan_surface.hpp"
#include "window/glfw_utils.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
VulkanContext::VulkanContext(const Window& window) : m_window{ window }
{
}

void VulkanContext::Init()
{
    InitInstance();
    InitDevice();
    InitMemoryAllocator();
    InitSwapchain();
}

void VulkanContext::Destroy()
{
    debug("Destroying VulkanContext");

    destroySwapchain(m_device, m_swapchain);
    vmaDestroyAllocator(m_allocator);
    destroyDevice(m_device);
    vkDestroySurfaceKHR(
        m_instance.handle,
        m_surface,
        allocationCallbacks.get());
    destroyInstance(m_instance);
}

void VulkanContext::ResizeSwapchain(const VkExtent2D& extent)
{
    m_swapchainBuilder.setOldSwapchain(m_swapchain.handle);
    m_swapchainBuilder.setDesiredExtent(extent.width, extent.height);

    const auto& result{ m_swapchainBuilder.build() };

    destroySwapchain(m_device, m_swapchain);
    m_swapchain = result.value();

    debug("Swapchain resized: {}x{}", extent.width, extent.height);
}

Instance& VulkanContext::GetInstance()
{
    return m_instance;
}

Device& VulkanContext::GetDevice()
{
    return m_device;
}

VmaAllocator& VulkanContext::GetAllocator()
{
    return m_allocator;
}

Swapchain& VulkanContext::GetSwapchain()
{
    return m_swapchain;
}

void VulkanContext::InitInstance()
{
    InstanceBuilder instanceBuilder;
    instanceBuilder.setApiVersion(VK_API_VERSION_1_3);
    instanceBuilder.setEngineName("Zeus");
    instanceBuilder.setEngineVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.setAppName(m_window.title);
    instanceBuilder.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.setExtensions(getRequiredGlobalExtensions());

    m_instance = instanceBuilder.build();
}

void VulkanContext::InitDevice()
{
    createVkSurfaceKHR(m_instance.handle, m_window.handle, m_surface);

    VkPhysicalDeviceFeatures requestedFeatures{};
    requestedFeatures.sampleRateShading = VK_TRUE;
    requestedFeatures.samplerAnisotropy = VK_TRUE;
    requestedFeatures.fillModeNonSolid = VK_TRUE;

    VkPhysicalDeviceVulkan13Features features1_3{};
    features1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features1_3.dynamicRendering = true;
    features1_3.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features1_2{};
    features1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features1_2.bufferDeviceAddress = true;
    features1_2.descriptorIndexing = true;
    features1_2.descriptorBindingPartiallyBound = true;
    features1_2.descriptorBindingVariableDescriptorCount = true;
    features1_2.runtimeDescriptorArray = true;

    PhysicalDeviceSelector physicalDeviceSelector(m_instance.handle);
    physicalDeviceSelector.setSurface(m_surface);
    physicalDeviceSelector.setPreferredType(
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    physicalDeviceSelector.requirePresent();
    physicalDeviceSelector.dedicatedTransferQueue();
    physicalDeviceSelector.dedicatedComputeQueue();
    physicalDeviceSelector.setFeatures(requestedFeatures);
    physicalDeviceSelector.setFeatures1_2(features1_2);
    physicalDeviceSelector.setFeatures1_3(features1_3);

    auto physicalDevice = physicalDeviceSelector.select().value();

    DeviceBuilder deviceBuilder(physicalDevice);
    m_device = deviceBuilder.build();
}

void VulkanContext::InitMemoryAllocator()
{
    VmaAllocatorCreateInfo createInfo{};
    createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = m_device.physicalDevice;
    createInfo.device = m_device.logicalDevice;
    createInfo.instance = m_instance.handle;
    createInfo.pAllocationCallbacks = allocationCallbacks.get();

    vmaCreateAllocator(&createInfo, &m_allocator);
}

void VulkanContext::InitSwapchain()
{
    m_swapchainBuilder = SwapchainBuilder{};
    m_swapchainBuilder.setDevice(m_device, m_surface);
    m_swapchainBuilder.setDesiredExtent(
        m_window.extent.width,
        m_window.extent.height);
    m_swapchainBuilder.setDesiredPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
    m_swapchainBuilder.setImageUsageFlags(
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    m_swapchain = m_swapchainBuilder.build().value();
}
}
