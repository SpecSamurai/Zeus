#include "Sampler.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>

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
    std::string_view name)
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

    VKCHECK(
        vkCreateSampler(
            VkContext::GetLogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create sampler.");

    VkContext::SetDebugName(VK_OBJECT_TYPE_SAMPLER, m_handle, name);
}

Sampler::Sampler(Sampler&& other) noexcept
    : m_handle{ other.m_handle },
      m_magFilter{ other.m_magFilter },
      m_minFilter{ other.m_minFilter },
      m_mipmapMode{ other.m_mipmapMode },
      m_addressMode{ other.m_addressMode },
      m_mipLodBias{ other.m_mipLodBias },
      m_maxAnisotropy{ other.m_maxAnisotropy },
      m_compareEnable{ other.m_compareEnable },
      m_compareOp{ other.m_compareOp }
{
    other.m_handle = VK_NULL_HANDLE;
}

Sampler& Sampler::operator=(Sampler&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_magFilter = other.m_magFilter;
        m_minFilter = other.m_minFilter;
        m_mipmapMode = other.m_mipmapMode;
        m_addressMode = other.m_addressMode;
        m_mipLodBias = other.m_mipLodBias;
        m_maxAnisotropy = other.m_maxAnisotropy;
        m_compareEnable = other.m_compareEnable;
        m_compareOp = other.m_compareOp;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

Sampler::~Sampler()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::Sampler, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void Sampler::Destroy()
{
    vkDestroySampler(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
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
