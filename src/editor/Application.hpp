#pragma once

#include <vulkan/VulkanContext.hpp>

namespace Zeus
{
class Application
{
public:
    void Init(float width, float height, const char* title);
    void Run();
    void Shutdown();
};
}
