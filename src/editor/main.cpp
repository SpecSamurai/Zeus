#include "VulkanContext.hpp"

#include <cstdlib>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Zeus::VulkanContext vkContext;

    vkContext.init(1440, 1080, "Zeus");

    vkContext.run();

    vkContext.destroy();

    return EXIT_SUCCESS;
}
