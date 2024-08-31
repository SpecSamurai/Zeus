#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class DescriptorAllocator
{
public:
    VkDescriptorSet allocate(
        VkDevice device,
        VkDescriptorSetLayout descriptorSetLayout);

    void init(
        VkDevice device,
        std::uint32_t maxSets,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        VkDescriptorPoolCreateFlags flags = 0);

    void clearDescriptors(VkDevice device);
    void destroyPool(VkDevice device);

private:
    VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
};
}
