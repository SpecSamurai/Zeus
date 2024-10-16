#include "CommandLineArgs.hpp"

#include <cassert>

namespace Zeus
{
CommandLineArgs::CommandLineArgs(char** argv, int argc)
    : m_argv{ argv },
      m_argc{ argc }
{
}

const char* CommandLineArgs::operator[](int index) const
{
    assert(index < m_argc);
    return m_argv[index];
}
}
