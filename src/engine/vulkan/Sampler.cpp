#include "Sampler.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "api/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
Sampler::Sampler(
    VkFilter magFilter,
    VkFilter minFilter,
    VkSamplerMipmapMode mipmapMode,
    VkSamplerAddressMode addressMode,
    float mipLodBias,
    float maxAnisotropy,
    VkBool32 compareEnable,
    VkCompareOp compareOp,
    const char* name)
{
    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.magFilter = magFilter;
    createInfo.minFilter = minFilter;
    createInfo.mipmapMode = mipmapMode;
    createInfo.addressModeU = addressMode;
    createInfo.addressModeV = addressMode;
    createInfo.addressModeW = addressMode;
    createInfo.mipLodBias = mipLodBias;
    createInfo.anisotropyEnable = maxAnisotropy != 0.f;
    createInfo.maxAnisotropy = maxAnisotropy;
    createInfo.compareEnable = compareEnable;
    createInfo.compareOp = compareOp;
    createInfo.minLod = 0.f;
    createInfo.maxLod = VK_LOD_CLAMP_NONE;
    createInfo.borderColor =
        VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK; // VK_BORDER_COLOR_INT_OPAQUE_BLACK
    createInfo.unnormalizedCoordinates = VK_FALSE;

    VkResult result{ vkCreateSampler(
        VkContext::GetLogicalDevice(),
        &createInfo,
        allocationCallbacks.get(),
        &m_handle) };

    VKCHECK(result, "Failed to create sampler.");

#ifndef NDEBUG
    if (name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_SAMPLER,
            reinterpret_cast<std::uint64_t>(m_handle),
            name);
    }
#endif
}

Sampler::~Sampler()
{
    VkContext::GetDevice().GetDeletionQueue().Add(
        ResourceType::Sampler,
        m_handle);
}

VkSampler Sampler::GetHandle() const
{
    return m_handle;
}

VkFilter Sampler::GetFilterMin() const
{
    return m_minFilter;
}

VkFilter Sampler::GetFilterMag() const
{
    return m_magFilter;
}

VkSamplerMipmapMode Sampler::GetMipmapMode() const
{
    return m_mipmapMode;
}

VkSamplerAddressMode Sampler::GetAddressMode() const
{
    return m_addressMode;
}

bool Sampler::IsAnisotrpyEnabled() const
{
    return m_maxAnisotropy != 0.f;
}

float Sampler::GetMaxAnisotropy() const
{
    return m_maxAnisotropy;
}

VkCompareOp Sampler::GetCompareOp() const
{
    return m_compareOp;
}
}
