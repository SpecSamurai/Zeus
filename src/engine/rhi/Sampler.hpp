#pragma once

#include <vulkan/vulkan_core.h>

#include <string_view>

namespace Zeus
{
class Sampler
{
public:
    Sampler() = default;

    Sampler(
        std::string_view name,
        VkFilter magFilter,
        VkFilter minFilter,
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        float mipLodBias = 0.f,
        float maxAnisotropy = 0.f,
        VkBool32 compareEnable = VK_FALSE,
        VkCompareOp compareOp = VK_COMPARE_OP_NEVER);

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    Sampler(Sampler&& other) noexcept;
    Sampler& operator=(Sampler&& other);

    ~Sampler();

    void Destroy();

    VkSampler GetHandle() const;
    VkFilter GetFilterMin() const;
    VkFilter GetFilterMag() const;
    VkSamplerMipmapMode GetMipmapMode() const;
    VkSamplerAddressMode GetAddressMode() const;

    bool IsAnisotrpyEnabled() const;
    float GetMaxAnisotropy() const;

    VkCompareOp GetCompareOp() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkSampler m_handle{ VK_NULL_HANDLE };
    std::string_view m_name;

    VkFilter m_magFilter;
    VkFilter m_minFilter;
    VkSamplerMipmapMode m_mipmapMode;
    VkSamplerAddressMode m_addressMode;
    float m_mipLodBias;
    float m_maxAnisotropy;
    VkBool32 m_compareEnable;
    VkCompareOp m_compareOp;
};
}
