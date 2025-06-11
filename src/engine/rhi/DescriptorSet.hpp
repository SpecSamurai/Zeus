#pragma once

#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
class DescriptorSet
{
public:
    DescriptorSet() = default;
    DescriptorSet(
        std::string_view name,
        const DescriptorSetLayout& descriptorSetLayout,
        DescriptorPool& descriptorPool);

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    DescriptorSet(DescriptorSet&& other) noexcept;
    DescriptorSet& operator=(DescriptorSet&& other);

    void Update(
        const std::vector<VkDescriptorBufferInfo>& imageInfos,
        std::uint32_t dstBinding,
        VkDescriptorType descriptorType,
        std::uint32_t dstArrayElement = 0);

    void Update(
        const std::vector<VkDescriptorImageInfo>& imageInfos,
        std::uint32_t dstBinding,
        VkDescriptorType descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        std::uint32_t dstArrayElement = 0);

    VkDescriptorSet GetHandle() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkDescriptorSet m_handle{ VK_NULL_HANDLE };

    std::string_view m_name;
};
}
