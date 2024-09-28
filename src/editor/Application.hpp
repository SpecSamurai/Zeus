#pragma once

#include <vulkan/VulkanContext.hpp>

#include <cstdint>

namespace Zeus
{
class Application
{
public:
    Application(std::uint32_t width, std::uint32_t height, const char* title);
    void Init();
    void Run();
    void Shutdown();

private:
};
}
