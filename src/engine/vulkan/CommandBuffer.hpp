#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class CommandBuffer
{
public:
    // void Dispatch(
    //     uint32_t groupCountX,
    //     uint32_t groupCountY,
    //     uint32_t groupCountZ)
    // {
    //     assert(m_type == QueueType::Compute);
    //     // commandBuffer must be in the recording state
    //
    //     vkCmdDispatch(m_handle, groupCountX, groupCountY, groupCountZ);
    // }

    void InsertImageLayoutBarrier(VkImageLayout layout)
    {
    }
};
}
