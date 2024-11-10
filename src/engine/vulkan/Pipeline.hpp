#pragma once

#include "DescriptorSetLayout.hpp"
#include "PipelineState.hpp"
#include "PushConstants.hpp"

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

namespace Zeus
{
class Pipeline
{
public:
    Pipeline(
        const PipelineState& pipelineState,
        const std::vector<DescriptorSetLayout>& descriptorSetLayouts,
        const std::vector<PushConstants>& pushConstants,
        const char* name = nullptr);

    ~Pipeline();

    VkPipeline GetHandle() const;
    VkPipelineLayout GetLayout() const;
    const PipelineState& GetState() const;
    const std::string& GetName() const;

private:
    void CreateGraphicsPipeline();
    void CreateComputePipeline();

private:
    VkPipeline m_handle{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
    const PipelineState& m_state;
    std::string m_name;
};
}
