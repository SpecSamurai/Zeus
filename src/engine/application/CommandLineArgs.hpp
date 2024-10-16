#pragma once

namespace Zeus
{
class CommandLineArgs
{
public:
    CommandLineArgs(char** argv, int argc);

    const char* operator[](int index) const;

private:
    char** m_argv{ nullptr };
    int m_argc{ 0 };
};
}
