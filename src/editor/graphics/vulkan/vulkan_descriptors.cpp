#include "vulkan_descriptors.hpp"

#include "core/logger.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include <array>

namespace Zeus
{
bool createDescriptorPool(
    const VkDevice& device,
    VkDescriptorPool& descriptorPool)
{
    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = FRAMES_IN_FLIGHT;
    // poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // poolSizes[1].descriptorCount = FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = FRAMES_IN_FLIGHT;

    VkResult result{
        vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create descriptor pool. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool createDescriptorSetLayout(
    const VkDevice& device,
    VkDescriptorSetLayout& descriptorSetLayout)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    // samplerLayoutBinding.binding = 1;
    // samplerLayoutBinding.descriptorCount = 1;
    // samplerLayoutBinding.descriptorType =
    //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // samplerLayoutBinding.pImmutableSamplers = nullptr;
    // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings{
        uboLayoutBinding,
        // samplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
    createInfo.pBindings = bindings.data();

    VkResult result{ vkCreateDescriptorSetLayout(
        device,
        &createInfo,
        nullptr,
        &descriptorSetLayout) };

    if (result != VK_SUCCESS)
    {
        error(
            "Failed to create descriptor set layout. {}",
            vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
