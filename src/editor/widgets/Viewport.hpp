#pragma once

#include "UserInterface.hpp"
#include "Widget.hpp"

#include <rhi/Image.hpp>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
class Viewport : public Widget
{
public:
    Viewport(UserInterface* root);

    void Initialize() override;
    void Update() override;

private:
    void ShowToolbar(bool show);
    void ShowStatistics(bool show);

private:
    UserInterface* m_root;
    const Image* m_image{ nullptr };
    VkDescriptorSet m_descriptorSet{ VK_NULL_HANDLE };
};
}
