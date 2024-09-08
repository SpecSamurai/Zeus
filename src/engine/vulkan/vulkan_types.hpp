#pragma once

#include "DeletionQueue.hpp"
#include "DynamicDescriptorAllocator.hpp"
#include "math/definitions.hpp"
#include "vulkan_image.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
namespace Colors
{
const static Vector4f RED{ Vector4f(1.f, 0.f, 0.f, 1.f) };
const static Vector4f GREEN{ Vector4f(0.f, 1.f, 0.f, 1.f) };
}

struct FrameData
{
    DeletionQueue deletionQueue;

    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;

    VkSemaphore imageAcquiredSemaphore;
    VkSemaphore renderCompleteSemaphore;

    VkFence renderFence;

    DynamicDescriptorAllocator frameDescriptors;

    // TODO: conisider adding draw resources per frame
    // Image Backbuffer;
};
}
