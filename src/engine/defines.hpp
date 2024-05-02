#pragma once

#ifndef _WIN64
#error "64-bit is required on Windows";
#endif

// #ifdef API_EXPORT
// #ifdef _MSC_VER
// #define API __declspec(dllexport)
// #else
// #define API __attribute((visibility("default")))
// #endif
// #else
// #ifdef _MSC_VER
// #define API __declspec(dllimport)
// #else
// #define API
// #endif
// #endif
