#include "Stopwatch.hpp"

#include <cassert>
#include <chrono>

namespace Zeus
{
void Stopwatch::Restart()
{
    m_start = std::chrono::high_resolution_clock::now();
}

double Stopwatch::GetElapsedMilliseconds()
{
    auto endTime{ std::chrono::high_resolution_clock::now() };
    auto elapsed{ std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - m_start) };

    return 0.001f * static_cast<double>(elapsed.count());
}
}
