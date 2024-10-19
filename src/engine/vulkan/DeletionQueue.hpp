#pragma once

#include "Definitions.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Zeus
{
class DeletionQueue
{
public:
    DeletionQueue(VkDevice device);

    void Add(const ResourceType type, void* handle);
    void Clear();

private:
    VkDevice m_device;
    std::unordered_map<ResourceType, std::vector<void*>> m_resources;
    std::mutex m_mutex;
};
}
