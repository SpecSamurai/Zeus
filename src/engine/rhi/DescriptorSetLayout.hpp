#pragma once

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
        const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings,
        VkDescriptorSetLayoutCreateFlags flags = 0,
        const std::vector<VkDescriptorBindingFlags>& bindingFlags = {});

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other);

    ~DescriptorSetLayout();

    void Destroy();

    VkDescriptorSetLayout GetHandle() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkDescriptorSetLayout m_handle{ VK_NULL_HANDLE };

    std::string_view m_name;
};
}
