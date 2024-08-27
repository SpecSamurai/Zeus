#include "vulkan_memory.hpp"

#include "core/logger.hpp"

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
    debug("allocationFunction");
    return nullptr;
}

void freeFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] void* pMemory)
{
    debug("freeFunction");
}

void internalAllocationNotification(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] VkInternalAllocationType allocationType,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    debug("internalAllocationNotification");
}

void internalFreeNotification(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] VkInternalAllocationType allocationType,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    debug("internalFreeNotification");
}

void* reallocationFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] void* pOriginal,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] std::size_t alignment,
    [[maybe_unused]] VkSystemAllocationScope allocationScope)
{
    debug("reallocationFunction");
    return nullptr;
}
}
