#pragma once

#include "DescriptorSetLayout.hpp"
#include "PipelineState.hpp"
#include "PushConstants.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>
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
        std::string_view name = "");

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Pipeline&& other) noexcept;
    Pipeline& operator=(Pipeline&& other);

    ~Pipeline();

    void Destroy();

    VkPipeline GetHandle() const;
    VkPipelineLayout GetLayout() const;
    const PipelineState& GetState() const;
    std::string_view GetName() const;
    VkPipelineBindPoint GetBindPoint() const;

private:
    void CreateGraphicsPipeline();
    void CreateComputePipeline();
    VkPipelineShaderStageCreateInfo CreatePipelineShaderStageInfo(
        VkShaderStageFlagBits stage,
        VkShaderModule module,
        const char* pName) const;

private:
    VkPipeline m_handle{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
    const PipelineState& m_state;
    std::string_view m_name;
    VkPipelineBindPoint m_bindPoint;
};
}
