#pragma once

#include "logger.hpp"

#include <fstream>
#include <string>
#include <vector>

namespace Zeus
{
inline std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        error("Failed to open file: {}", filename);
        throw std::runtime_error("failed to open file!");
    }

    std::size_t fileSize = (std::size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), (std::streamsize)fileSize);

    file.close();

    return buffer;
}
}
