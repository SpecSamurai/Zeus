#pragma once

#include "vulkan/VulkanContext.hpp"

namespace Zeus
{
class Engine
{
public:
    Engine(std::uint32_t width, std::uint32_t height, const char* title);
    void Init();
    void Run();
    void Shutdown();

    VulkanContext vkContext;

private:
};
}
