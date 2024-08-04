#pragma once

#include "Device.hpp"
#include "PhysicalDevice.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class DeviceBuilder
{
public:
    DeviceBuilder(const PhysicalDevice& physicalDevice);

    std::optional<Device> build();

private:
    PhysicalDevice physicalDevice{};
};
}
