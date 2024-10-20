#pragma once

namespace Zeus
{
enum class ResourceType
{
    Fence,
    Semaphore,
    Count,
};

enum class QueueType
{
    Graphics,
    Present,
    Transfer,
    Compute,
};
}
