#pragma once

#include "vulkan/VulkanContext.hpp"

#include <imgui.h>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
class UIManager
{
public:
    void Init(const VulkanContext& vkContext);

    void ConfigureFrame();

    void Draw(
        VkCommandBuffer commandBuffer,
        const VkRenderingAttachmentInfo& colorAttachment,
        const VkExtent2D& renderExtent);

    void Destroy(VkDevice device);

private:
    void ShowStatistics(const ImGuiViewport* viewport, bool opened);
    void ShowMenuBar();
    void ShowToolbar();

    VkDescriptorPool uiDescriptorPool{ VK_NULL_HANDLE };

    bool showDemoWindow{ false };
};
}
