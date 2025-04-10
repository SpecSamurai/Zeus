#include "Profiler.hpp"

namespace Zeus
{
float Profiler::FPS()
{
    return s_fps;
}

float Profiler::LastFrametime()
{
    return s_lastFrametime;
}

float Profiler::AvgFrametime()
{
    return s_avgFrametime;
}

float Profiler::DrawTime()
{
    return s_drawTime;
}

float Profiler::UpdateTime()
{
    return s_updateTime;
}

std::int32_t Profiler::TriangleCount()
{
    return s_triangleCount;
}

std::int32_t Profiler::DrawCallCount()
{
    return s_drawCallCount;
}

std::uint32_t Profiler::GpuMemoryAvailable()
{
    return s_gpuMemoryAvailable;
}

std::uint32_t Profiler::GpuMemoryUsed()
{
    return s_gpuMemoryUsed;
}
}
