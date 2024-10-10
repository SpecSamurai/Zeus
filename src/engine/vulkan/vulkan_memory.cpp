#define VMA_IMPLEMENTATION
#include "vulkan_memory.hpp"

#include "logging/logger.hpp"

#include <vulkan/vulkan.h>

#include <cstddef>

namespace Zeus
{
void* allocationFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] std::size_t alignment,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    LOG_DEBUG("allocationFunction");
    return nullptr;
}

void freeFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] void* pMemory)
{
    LOG_DEBUG("freeFunction");
}

void internalAllocationNotification(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] VkInternalAllocationType allocationType,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    LOG_DEBUG("internalAllocationNotification");
}

void internalFreeNotification(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] VkInternalAllocationType allocationType,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    LOG_DEBUG("internalFreeNotification");
}

void* reallocationFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] void* pOriginal,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] std::size_t alignment,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    LOG_DEBUG("reallocationFunction");
    return nullptr;
}
}
