#pragma once

#include "Camera.hpp"
#include "vulkan/DeletionQueue.hpp"
#include "vulkan/DescriptorAllocator.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/DynamicDescriptorAllocator.hpp"
#include "vulkan/Instance.hpp"
#include "vulkan/SwapchainBuilder.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_types.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <cstdint>
#include <span>
#include <vector>

namespace Zeus
{
class VulkanContext
{
public:
    VulkanContext()
    {
        camera = Camera();
    }

    void init(std::uint32_t width, std::uint32_t height, const char* title);
    void run();
    void destroy();

private:
    void initWindow(int width, int height, const char* title);
    void initInstance(const char* applicationName);
    void initDevice();
    void initMemoryAllocator();
    void initSwapchain();
    void initDrawResources();
    void initCommands();
    void initSynchronizationObjects();

    void initPipelines();

    void initImGui();
    void renderUI();

    void resizeSwapchain();

    void draw();
    void drawGeometry(VkCommandBuffer commandBuffer);
    void drawUI(
        VkCommandBuffer commandBuffer,
        std::uint32_t swapchainImageIndex);

    // ************************
public:
    void update_scene();

    Image create_image(
        void* data,
        VkExtent3D size,
        VkFormat format,
        VkImageUsageFlags usage,
        bool mipmapped);

    // todo: consider using span instaed of ptr + size
    void initDescriptorSets();

    void generateMipmaps(
        VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        uint32_t mipLevels);

    MeshBuffers uploadMesh(
        std::span<uint32_t> indices,
        std::span<Vertex> vertices);

    void init_default_data();
    float lastX = 800 / 2.f;
    float lastY = 600 / 2.f;
    bool firstMouse = true;
    // ************************************

    void cmdOneTimeSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

    static void glfwKeyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods);

    static void glfwCursorPositionCallback(
        GLFWwindow* window,
        double xpos,
        double ypos);

    static void glfwScrollCallback(
        GLFWwindow* window,
        double xoffset,
        double yoffset);

    static void glfwWindowIconifyCallback(GLFWwindow* window, int iconified);

public:
    DeletionQueue deletionQueue;
    DescriptorAllocator descriptorAllocator;
    DynamicDescriptorAllocator dynamicDescriptorAllocator;
    SwapchainBuilder swapchainBuilder;

    // hmmmm
    Camera camera;

    static constexpr std::uint32_t MAX_CONCURRENT_FRAMES{ 2 };

    bool showDemoWindow{ false };
    bool stopRendering{ false };

    VkExtent2D windowExtent;
    bool resizeWindow{ false };
    GLFWwindow* window{ nullptr };

    Instance instance;
    Device device;
    VmaAllocator allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };
    Swapchain swapchain;

    float renderScale{ 1.f };
    VkExtent2D drawExtent;
    Image drawImage;
    Image depthImage;

    std::uint32_t currentFrame{ 0 };
    FrameData frames[MAX_CONCURRENT_FRAMES];
    constexpr FrameData& getCurrentFrame();

    VkCommandPool immediateCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer immediateCommandBuffer{ VK_NULL_HANDLE };
    VkFence immediateSubmitFence{ VK_NULL_HANDLE };

    VkDescriptorPool imGuiDescriptorPool;

    // TODO: Rename
    VkDescriptorSet drawImageDescriptorSet;
    VkDescriptorSetLayout drawImageDescriptorLayout;
    //***************************************************************

    VkPipelineLayout gradientPipelineLayout;

    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
};
}
