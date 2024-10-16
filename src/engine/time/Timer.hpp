#pragma once

#include <chrono>

namespace Zeus
{
class Timer
{
public:
    Timer();

    void Start();
    float GetElapsedMilliseconds();

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
};
}
