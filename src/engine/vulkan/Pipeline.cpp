#include "Pipeline.hpp"

#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "VkContext.hpp"
#include "vulkan/api/vulkan_debug.hpp"
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
    // DeletionQueue queue;
    // queue.Add(ResourceType::Pipeline, m_handle);
    // m_handle = VK_NULL_HANDLE;
    //
    // queue.Add(ResourceType::PipelineLayout, m_layout);
    // m_layout = VK_NULL_HANDLE;
}
}
