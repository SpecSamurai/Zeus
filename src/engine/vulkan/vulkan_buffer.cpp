#include "vulkan_buffer.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createBuffer(
    VmaAllocator allocator,
    Buffer& buffer,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags,
    bool map)
{
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.requiredFlags = memoryPropertyFlags;

    switch (memoryPropertyFlags)
    {
    case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;

    case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        break;

    case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        break;

    default:
        assert(false && "Unsupported Memory property flag.");
    }

    if (map)
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkResult result{ vmaCreateBuffer(
        allocator,
        &createInfo,
        &allocationCreateInfo,
        &buffer.buffer,
        &buffer.allocation,
        &buffer.info) };

    VKCHECK(result, "Failed to create buffer.");

    return result;
}

void cmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset,
    VkDeviceSize size)
{
    VkBufferCopy2 vertexBufferCopy{};
    vertexBufferCopy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    vertexBufferCopy.srcOffset = srcOffset;
    vertexBufferCopy.dstOffset = dstOffset;
    vertexBufferCopy.size = size;

    VkCopyBufferInfo2 vertexBufferCopyInfo{};
    vertexBufferCopyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    vertexBufferCopyInfo.pNext = nullptr;
    vertexBufferCopyInfo.srcBuffer = srcBuffer;
    vertexBufferCopyInfo.dstBuffer = dstBuffer;
    vertexBufferCopyInfo.regionCount = 1;
    vertexBufferCopyInfo.pRegions = &vertexBufferCopy;

    vkCmdCopyBuffer2(commandBuffer, &vertexBufferCopyInfo);
}

VkDeviceAddress getBufferDeviceAddress(VkDevice device, VkBuffer& buffer)
{
    VkBufferDeviceAddressInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.pNext = nullptr;
    info.buffer = buffer;

    return vkGetBufferDeviceAddress(device, &info);
}
}
