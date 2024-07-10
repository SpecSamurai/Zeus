#include "vulkan_memory.hpp"

#include "vulkan_command_buffer.hpp"
#include "vulkan_device.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
// Graphics cards can offer different types of memory to allocate from. Each
// type of memory varies in terms of allowed operations and performance
// characteristics. We need to combine the requirements of the buffer and
// our own application requirements to find the right type of memory to use.
std::uint32_t findMemoryType(
    const std::uint32_t& typeFilter,
    const VkPhysicalDevice& physicalDevice,
    const VkMemoryPropertyFlags& properties)
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

    error("Failed to find suitable memory type");
    return 0;
}

// When we queried for buffer memory requirement, we acquired information
// about required size, memory type, and alignment. Different buffer usages
// may require different memory alignment. The beginning of a memory object
// (offset of 0) satisfies all alignments. This means that all memory
// objects are created at addresses that fulfill the requirements of all
// different usages. So when we specify a zero offset, we don’t have to
// worry about anything.
// But we can create larger memory object and use it as a storage space for
// multiple buffers (or images). This, in fact, is the recommended behavior.
// Creating larger memory objects means we are creating fewer memory
// objects. This allows driver to track fewer objects in general. Memory
// objects must be tracked by a driver because of OS requirements and
// security measures. Larger memory objects don’t cause big problems with
// memory fragmentation. Finally, we should allocate larger memory amounts
// and keep similar objects in them to increase cache hits and thus improve
// performance of our application.

// But when we allocate larger memory objects and bind them to multiple
// buffers (or images), not all of them can be bound at offset zero. Only
// one can be bound at this offset, others must be bound further away, after
// a space used by the first buffer (or image). So the offset for the
// second, and all other buffers bound to the same memory object, must meet
// alignment requirements reported by the query. And we must remember it.
// That’s why alignment member is important.
void CreateBuffer(
    VulkanDevice device,
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

    if (vkCreateBuffer(device.logicalDevice, &bufferInfo, nullptr, &buffer) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    // The buffer has been created, but it doesn't actually have any memory
    // assigned to it yet. The first step of allocating memory for the
    // buffer is to query its memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(
        device.logicalDevice,
        buffer,
        &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits,
        device.physicalDevice,
        properties);

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
    if (vkAllocateMemory(
            device.logicalDevice,
            &allocInfo,
            nullptr,
            &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    // The first three parameters are self-explanatory and the fourth
    // parameter is the offset within the region of memory. Since this
    // memory is allocated specifically for this the vertex buffer, the
    // offset is simply 0. If the offset is non-zero, then it is required to
    // be divisible by memRequirements.alignment.
    vkBindBufferMemory(device.logicalDevice, buffer, bufferMemory, 0);
}

void copyBuffer(
    VulkanDevice device,
    VkCommandPool commandPool,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size)
{
    VkCommandBuffer commandBuffer =
        beginSingleTimeCommands(device.logicalDevice, commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(device, commandBuffer, commandPool);

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
}
