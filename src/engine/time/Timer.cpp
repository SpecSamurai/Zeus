#include "Timer.hpp"

#include <chrono>

namespace Zeus
{
Timer::Timer()
{
    Start();
}

void Timer::Start()
{
    m_start = std::chrono::high_resolution_clock::now();
}

float Timer::GetElapsedMilliseconds()
{
    auto elapsed{ std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - m_start) };

    return static_cast<float>(elapsed.count());
}
}
