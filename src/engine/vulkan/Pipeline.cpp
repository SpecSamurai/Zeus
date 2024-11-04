#include "Pipeline.hpp"

#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "VkContext.hpp"
#include "api/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
Pipeline::Pipeline(PipelineState pipelineState, const char* name)
{

#ifndef NDEBUG
    if (name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_PIPELINE,
            reinterpret_cast<std::uint64_t>(m_handle),
            name);

        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            reinterpret_cast<std::uint64_t>(m_pipelineLayout),
            name);
    }
#endif
}

Pipeline::~Pipeline()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        VkContext::GetDevice().GetDeletionQueue().Add(
            ResourceType::Pipeline,
            m_handle);
        m_handle = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        VkContext::GetDevice().GetDeletionQueue().Add(
            ResourceType::PipelineLayout,
            m_pipelineLayout);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}

VkPipeline Pipeline::GetHandle() const
{
    return m_handle;
}

VkPipelineLayout Pipeline::GetLayout() const
{
    return m_pipelineLayout;
}
}
