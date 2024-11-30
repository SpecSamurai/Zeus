#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
class DescriptorPool
{
public:
    DescriptorPool() = default;
    DescriptorPool(
        std::uint32_t maxSets,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        VkDescriptorPoolCreateFlags flags = {},
        std::string_view name = "");

    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(DescriptorPool&& other);

    void Destroy();

    VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout) const;
    void Reset() const;

    VkDescriptorPool GetHandle() const;

private:
    VkDescriptorPool m_handle{ VK_NULL_HANDLE };
};
}
