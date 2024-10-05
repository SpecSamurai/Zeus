#include "VulkanContext.hpp"

#include "Device.hpp"
#include "DeviceBuilder.hpp"
#include "Instance.hpp"
#include "InstanceBuilder.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "SwapchainBuilder.hpp"
#include "core/logger.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan_memory.hpp"
#include "vulkan_surface.hpp"
#include "window/glfw_utils.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
VulkanContext::VulkanContext(
    std::uint32_t width,
    std::uint32_t height,
    const char* title)
    : window(width, height, title)
{
}

void VulkanContext::Init()
{
    window.Init();
    InitInstance();
    InitDevice();
    InitMemoryAllocator();
    InitSwapchain();
}

void VulkanContext::Destroy()
{
    debug("Destroying VulkanContext");

    destroySwapchain(device, swapchain);
    vmaDestroyAllocator(allocator);
    destroyDevice(device);
    vkDestroySurfaceKHR(instance.handle, surface, allocationCallbacks.get());
    destroyInstance(instance);
    window.Destroy();
}

void VulkanContext::ResizeSwapchain(const VkExtent2D& extent)
{
    vkDeviceWaitIdle(device.logicalDevice);

    swapchainBuilder.setOldSwapchain(swapchain.handle);
    swapchainBuilder.setDesiredExtent(extent.width, extent.height);

    const auto& result{ swapchainBuilder.build() };

    destroySwapchain(device, swapchain);
    swapchain = result.value();

    debug("Swapchain resized: {}x{}", extent.width, extent.height);
}

void VulkanContext::InitInstance()
{
    InstanceBuilder instanceBuilder;
    instanceBuilder.setApiVersion(VK_API_VERSION_1_3);
    instanceBuilder.setEngineName("Zeus");
    instanceBuilder.setEngineVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.setAppName(window.title);
    instanceBuilder.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.setExtensions(getRequiredGlobalExtensions());

    instance = instanceBuilder.build();
}

void VulkanContext::InitDevice()
{
    createVkSurfaceKHR(instance.handle, window.handle, surface);

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

    PhysicalDeviceSelector physicalDeviceSelector(instance.handle);
    physicalDeviceSelector.setSurface(surface);
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
    device = deviceBuilder.build();
}

void VulkanContext::InitMemoryAllocator()
{
    VmaAllocatorCreateInfo createInfo{};
    createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = device.physicalDevice;
    createInfo.device = device.logicalDevice;
    createInfo.instance = instance.handle;
    createInfo.pAllocationCallbacks = allocationCallbacks.get();

    vmaCreateAllocator(&createInfo, &allocator);
}

void VulkanContext::InitSwapchain()
{
    swapchainBuilder = SwapchainBuilder{};
    swapchainBuilder.setDevice(device, surface);
    swapchainBuilder.setDesiredExtent(
        window.extent.width,
        window.extent.height);
    swapchainBuilder.setDesiredPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
    swapchainBuilder.setImageUsageFlags(
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    swapchain = swapchainBuilder.build().value();
}
}
