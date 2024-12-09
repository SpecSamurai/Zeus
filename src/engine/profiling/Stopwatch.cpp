#include "Stopwatch.hpp"

#include <cassert>
#include <chrono>

namespace Zeus
{
void Stopwatch::Restart()
{
    m_start = std::chrono::steady_clock::now();
}

double Stopwatch::GetElapsedMilliseconds()
{
    auto endTime{ std::chrono::steady_clock::now() };
    auto elapsed{ std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - m_start) };

    return static_cast<double>(elapsed.count());
}
}
