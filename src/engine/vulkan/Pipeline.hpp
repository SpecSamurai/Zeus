#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class PipelineState
{
public:
private:
};

class Pipeline
{
public:
    Pipeline(PipelineState pipelineState, const char* name = nullptr);
    ~Pipeline();

    VkPipeline GetHandle() const;
    VkPipelineLayout GetLayout() const;

private:
    VkPipeline m_handle{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
};
}
