#pragma once

#include "DescriptorSetLayout.hpp"
#include "PipelineState.hpp"
#include "PushConstants.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
class Pipeline
{
public:
    Pipeline(
        const PipelineState& pipelineState,
        const std::vector<DescriptorSetLayout*>& descriptorSetLayouts,
        const std::vector<PushConstants>& pushConstants,
        const char* name = nullptr);

    ~Pipeline();

    void Destroy();

    VkPipeline GetHandle() const;
    VkPipelineLayout GetLayout() const;
    const PipelineState& GetState() const;
    const char* GetName() const;

private:
    void CreateGraphicsPipeline();
    void CreateComputePipeline();

private:
    VkPipeline m_handle{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
    const PipelineState& m_state;
    const char* m_name;
};
}
