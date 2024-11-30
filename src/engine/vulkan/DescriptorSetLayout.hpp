#pragma once

#include "Descriptor.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>
#include <vector>

namespace Zeus
{
class DescriptorSetLayout
{
public:
    DescriptorSetLayout() = default;

    DescriptorSetLayout(
        const std::vector<Descriptor>& descriptors,
        std::string_view name = "");

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other);

    ~DescriptorSetLayout();

    void Destroy();

    const VkDescriptorSetLayout& GetHandle() const;
    const std::vector<Descriptor>& GetDescriptors() const;

private:
    VkDescriptorSetLayout m_handle{ VK_NULL_HANDLE };
    std::vector<Descriptor> m_descriptors;
};
}
