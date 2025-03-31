#pragma once

#include "Widget.hpp"

#include <rhi/Image.hpp>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
class Viewport : public Widget
{
public:
    void Initialize() override;
    void Update() override;

private:
    const Image* m_image{ nullptr };
    VkDescriptorSet m_descriptorSet{ VK_NULL_HANDLE };
};
}
