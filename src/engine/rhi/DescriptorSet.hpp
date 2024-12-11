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
        DescriptorSetLayout& descriptorSetLayout,
        DescriptorPool& descriptorPool,
        std::string_view name = "");

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    DescriptorSet(DescriptorSet&& other) noexcept;
    DescriptorSet& operator=(DescriptorSet&& other);

    void Update(
        const std::vector<DescriptorBuffer>& bufferDescriptors,
        const std::vector<DescriptorImage>& imageDescriptors);

    VkDescriptorSet GetHandle() const;

private:
    VkDescriptorSet m_handle{ VK_NULL_HANDLE };
    std::uint32_t m_binding;
};
}
