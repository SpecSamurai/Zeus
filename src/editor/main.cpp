#include "VulkanContext.hpp"

#include <cstdlib>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Zeus::VulkanContext vkContext;

    vkContext.Init(800, 600, "Zeus");

    while (!glfwWindowShouldClose(vkContext.window))
    {
        glfwPollEvents();
        vkContext.DrawFrame();
        // lastTime = glfwGetTime();
    }

    // Wait for operations in a specific command queue to be finished.
    // Can be used as a rudimentary synchronization
    vkDeviceWaitIdle(vkContext.device.logicalDevice);
    vkContext.Destroy();

    return EXIT_SUCCESS;
}
