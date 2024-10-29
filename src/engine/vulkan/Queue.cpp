#include "Queue.hpp"

#include "Definitions.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "Swapchain.hpp"
#include "VkContext.hpp"
#include "api/vulkan_command.hpp"
#include "api/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>

namespace Zeus
{
Queue::Queue(const QueueType type, const char* name) : m_type{ type }
{
    auto& device{ VkContext::GetDevice() };
    m_family = device.GetQueueFamily(m_type);
    m_handle = device.GetQueue(type);

#ifndef NDEBUG
    if (name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            device.GetLogicalDevice(),
            VK_OBJECT_TYPE_QUEUE,
            reinterpret_cast<std::uint64_t>(m_handle),
            name);
    }
#endif
}

Queue::~Queue()
{
    Wait();
}

void Queue::Wait()
{
    VKCHECK(vkQueueWaitIdle(m_handle), "Failed to wait for Queue");
}

void Queue::Submit(
    VkCommandBuffer commandBuffer,
    const Semaphore& waitSemaphore,
    const Semaphore& signalSemaphore,
    const Fence& fence)
{
    assert(m_type == QueueType::Graphics || m_type == QueueType::Transfer);

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        commandBuffer) };

    VkSemaphoreSubmitInfo waitSemaphoreInfo{
        createVkSemaphoreSubmitInfo(
            waitSemaphore.GetHandle(),
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
    }; // todo: adjust based on the queue

    VkSemaphoreSubmitInfo signalSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        signalSemaphore.GetHandle(),
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) };

    cmdVkQueueSubmit2(
        m_handle,
        fence.GetHandle(),
        1,
        &waitSemaphoreInfo,
        1,
        &submitInfo,
        1,
        &signalSemaphoreInfo);
}

void Queue::Present(
    const Swapchain& swapchain,
    std::uint32_t swapchainImageIndex,
    const Semaphore& waitSemaphores)
{
    assert(m_type == QueueType::Present);

    VkResult presentResult{ cmdVkQueuePresentKHR(
        m_handle,
        1,
        &waitSemaphores.GetHandle(),
        1,
        &swapchain.GetHandle(),
        &swapchainImageIndex) };

    // send event?

    // if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
    //     presentResult == VK_SUBOPTIMAL_KHR)
    // {
    //     m_swapchainRebuildRequired = true;
    // }
    // else if (presentResult != VK_SUCCESS)
    // {
    //     LOG_ERROR("Failed to present swapchain image");
    //     return presentResult;
    // }
}

const VkQueue& Queue::GetHandle() const
{
    return m_handle;
}

QueueType Queue::GetType() const
{
    return m_type;
}

std::uint32_t Queue::GetFamilyIndex() const
{
    return m_family;
}
}
