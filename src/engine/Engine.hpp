#pragma once

#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_image.hpp"
#include "vulkan/vulkan_types.hpp"

#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
class Engine
{
public:
    Engine(std::uint32_t width, std::uint32_t height, const char* title);
    void Init();
    void Run();
    void Shutdown();

    void cmdOneTimeSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

    VulkanContext vkContext;

private:
    void Draw();
    void InitSyncObjects();
    void InitCommands();

    inline constexpr FrameData& getCurrentFrame()
    {
        return frames[currentFrame];
    }

private:
    std::vector<FrameData> frames;
    std::uint32_t currentFrame{ 0 };

    VkExtent2D drawExtent;
    Image drawImage;
    Image depthImage;

    VkFence oneTimeSubmitFence{ VK_NULL_HANDLE };
    VkCommandPool oneTimeSubmitCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer oneTimeSubmitCommandBuffer{ VK_NULL_HANDLE };

    float renderScale{ 1.f };

    bool stopRendering{ false };
};
}
