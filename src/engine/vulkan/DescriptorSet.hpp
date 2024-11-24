#pragma once

#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class DescriptorSet
{
public:
    DescriptorSet() = default;
    DescriptorSet(
        const std::vector<Descriptor>& descriptors,
        DescriptorSetLayout& descriptorSetLayout,
        DescriptorPool& descriptorPool,
        const char* name = nullptr);

    void Update(const std::vector<Descriptor>& descriptors);

    VkDescriptorSet GetHandle() const;
    const std::vector<Descriptor>& GetDescriptors() const;

private:
    VkDescriptorSet m_handle{ VK_NULL_HANDLE };
    std::vector<Descriptor> m_descriptors;

    std::uint32_t m_binding;
};
}
