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
        std::string_view name,
        std::uint32_t maxSets,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        VkDescriptorPoolCreateFlags flags = {});

    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(DescriptorPool&& other);

    void Destroy();

    VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout) const;
    void Reset() const;

    VkDescriptorPool GetHandle() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkDescriptorPool m_handle{ VK_NULL_HANDLE };
    std::string_view m_name;
};
}
