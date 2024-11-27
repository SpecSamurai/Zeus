#pragma once

#include <string>

#if defined(_WIN32)
#define PLATFORM_WINDOWS
#ifndef _WIN64
#error "64-bit is required on Windows"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX
#elif __APPLE__
#define PLATFORM_APPLE
#else
#error "Unknown platform"
#endif

#ifdef API_EXPORT
#ifdef _MSC_VER
#define API __declspec(dllexport)
#else
#define API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define API __declspec(dllimport)
#else
#define API
#endif
#endif

namespace Zeus
{
inline const std::string AssetsFolderPath{ "../engine/assets" };
}
