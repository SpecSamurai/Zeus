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
        std::string_view name,
        const std::vector<Descriptor>& descriptors);

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other);

    ~DescriptorSetLayout();

    void Destroy();

    VkDescriptorSetLayout GetHandle() const;
    const std::vector<Descriptor>& GetDescriptors() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkDescriptorSetLayout m_handle{ VK_NULL_HANDLE };
    std::vector<Descriptor> m_descriptors;

    std::string_view m_name;
};
}
