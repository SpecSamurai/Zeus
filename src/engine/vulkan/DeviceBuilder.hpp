#pragma once

#include "Device.hpp"
#include "PhysicalDevice.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
class DeviceBuilder
{
public:
    DeviceBuilder(const PhysicalDevice& physicalDevice);

    Device build();

private:
    PhysicalDevice physicalDevice{};
};
}
