#pragma once

#include "Stopwatch.hpp"
#include "logging/logger.hpp"

#include <cstdint>

namespace Zeus
{
class Profiler
{
public:
    static float FPS();
    static float LastFrametime();
    static float AvgFrametime();
    static float DrawTime();
    static float UpdateTime();

    static std::int32_t TriangleCount();
    static std::int32_t DrawCallCount();

    static std::uint32_t GpuMemoryAvailable();
    static std::uint32_t GpuMemoryUsed();

    static void UpdateGpuMetrics();

    static void Begin()
    {
        ++frameCounter;
        s_triangleCount = 0;
        s_drawCallCount = 0;

        s_stopwatch.Restart();
    }

    static void End()
    {
        s_frametimeDelta = s_stopwatch.GetElapsedMilliseconds();
        s_lastFrametime = s_stopwatch.GetElapsedMilliseconds();

        if (frameCounter % 1000 == 0)
        {
            frameCounter = 0;
            s_fps = 1000.f / s_lastFrametime;
        }
    }

public:
    inline static Stopwatch s_stopwatch{};

    inline static int frameCounter{};

    inline static float s_fps{};
    inline static float s_lastFrametime{};
    inline static float s_avgFrametime{};
    inline static double s_frametimeDelta{};
    inline static float s_drawTime{};
    inline static float s_updateTime{};

    inline static std::int32_t s_triangleCount{};
    inline static std::int32_t s_drawCallCount{};

    inline static std::uint32_t s_gpuMemoryAvailable{};
    inline static std::uint32_t s_gpuMemoryUsed{};
};
}
