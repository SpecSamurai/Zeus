#pragma once

#include <chrono>

namespace Zeus
{
class Stopwatch
{
public:
    void Restart();

    double GetElapsedMilliseconds();

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
};
}
