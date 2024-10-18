#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-private-field"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wnullability-extension"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include <vk_mem_alloc.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <vulkan/vulkan.h>

#include <cstddef>
#include <memory>

namespace Zeus
{
void* allocationFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] std::size_t alignment,
    [[maybe_unused]] VkSystemAllocationScope allocationScope);

void freeFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] void* pMemory);

void internalAllocationNotification(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] VkInternalAllocationType allocationType,
    [[maybe_unused]] VkSystemAllocationScope allocationScope);

void internalFreeNotification(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] VkInternalAllocationType allocationType,
    [[maybe_unused]] VkSystemAllocationScope allocationScope);

void* reallocationFunction(
    [[maybe_unused]] void* pUserData,
    [[maybe_unused]] void* pOriginal,
    [[maybe_unused]] std::size_t size,
    [[maybe_unused]] std::size_t alignment,
    [[maybe_unused]] VkSystemAllocationScope allocationScope);

#ifndef NDEBUG
inline std::unique_ptr<VkAllocationCallbacks> allocationCallbacks{};
#else
inline std::unique_ptr<VkAllocationCallbacks> allocationCallbacks =
    std::make_unique<VkAllocationCallbacks>(VkAllocationCallbacks{
        .pUserData = {},
        .pfnAllocation = allocationFunction,
        .pfnReallocation = reallocationFunction,
        .pfnFree = freeFunction,
        .pfnInternalAllocation = internalAllocationNotification,
        .pfnInternalFree = internalFreeNotification,
    });
#endif
}
