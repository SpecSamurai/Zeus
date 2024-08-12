#pragma once

#include "core/logger.hpp"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
std::uint32_t findMemoryType(
    const VkPhysicalDevice& physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

class MemoryAllocator
{
#ifndef NDEBUG
public:
    inline static VkAllocationCallbacks* pAllocator{ nullptr };
#else
    inline static VkAllocationCallbacks* createAllocationCallbacks()
    {
        VkAllocationCallbacks* callbacks = new VkAllocationCallbacks{};
        callbacks->pUserData = {};
        callbacks->pfnAllocation = allocationFunction;
        callbacks->pfnReallocation = reallocationFunction;
        callbacks->pfnFree = freeFunction;
        callbacks->pfnInternalAllocation = internalAllocationNotification;
        callbacks->pfnInternalFree = internalFreeNotification;

        return callbacks;
    }

public:
    inline static VkAllocationCallbacks* pAllocator{
        createAllocationCallbacks()
    };

    // TODO: doesn't make sense deallocate static differently
    ~MemoryAllocator()
    {
        delete pAllocator;
        pAllocator = nullptr;
    }

private:
    static void* allocationFunction(
        [[maybe_unused]] void* pUserData,
        [[maybe_unused]] size_t size,
        [[maybe_unused]] size_t alignment,
        [[maybe_unused]] VkSystemAllocationScope allocationScope)
    {
        debug("allocationFunction");
        return nullptr;
    }

    static void freeFunction(
        [[maybe_unused]] void* pUserData,
        [[maybe_unused]] void* pMemory)
    {
    }

    static void internalAllocationNotification(
        [[maybe_unused]] void* pUserData,
        [[maybe_unused]] size_t size,
        [[maybe_unused]] VkInternalAllocationType allocationType,
        [[maybe_unused]] VkSystemAllocationScope allocationScope)
    {
        debug("internalAllocationNotification");
    }

    static void internalFreeNotification(
        [[maybe_unused]] void* pUserData,
        [[maybe_unused]] size_t size,
        [[maybe_unused]] VkInternalAllocationType allocationType,
        [[maybe_unused]] VkSystemAllocationScope allocationScope)
    {
        debug("internalFreeNotification");
    }

    static void* reallocationFunction(
        [[maybe_unused]] void* pUserData,
        [[maybe_unused]] void* pOriginal,
        [[maybe_unused]] size_t size,
        [[maybe_unused]] size_t alignment,
        [[maybe_unused]] VkSystemAllocationScope allocationScope)
    {
        debug("reallocationFunction");
        return nullptr;
    }
#endif
};
}
