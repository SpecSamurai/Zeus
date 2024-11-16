#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Zeus
{
class DescriptorPool
{
public:
    DescriptorPool(const char* name = "");
    ~DescriptorPool();

    void Destroy();

    VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout);

    void Init(
        std::uint32_t maxSets,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        VkDescriptorPoolCreateFlags flags = 0);

    void Reset();

    VkDescriptorPool GetHandle() const;

private:
    VkDescriptorPool m_handle{ VK_NULL_HANDLE };
    std::string m_name;
};
}
