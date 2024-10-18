#pragma once

#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/format.h>

#include <cassert>
#include <source_location>
#include <utility>

namespace Zeus
{
enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

template <typename... Args>
inline void log(
    LogLevel logLevel,
    const fmt::format_string<Args...>& message,
    const std::source_location& location,
    Args&&... args)
{
    fmt::text_style textStyle{};
    const char* logLevel_str{ nullptr };

    switch (logLevel)
    {
    case LogLevel::Trace:
        textStyle = fg(fmt::color::forest_green);
        logLevel_str = "TRACE";
        break;
    case LogLevel::Debug:
        textStyle = fg(fmt::color::dodger_blue);
        logLevel_str = "DEBUG";
        break;
    case LogLevel::Info:
        logLevel_str = "INFO";
        break;
    case LogLevel::Warning:
        textStyle = fg(fmt::color::golden_rod);
        logLevel_str = "WARNING";
        break;
    case LogLevel::Error:
        textStyle = fg(fmt::color::crimson);
        logLevel_str = "ERROR";
        break;
    case LogLevel::Fatal:
        textStyle = bg(fmt::color::crimson);
        logLevel_str = "FATAL";
        break;
    default:
        assert(false && "Unknown LogLevel");
    }

    switch (logLevel)
    {
    case LogLevel::Trace:
    case LogLevel::Debug:
    case LogLevel::Info:
    case LogLevel::Warning:
        fmt::print(
            textStyle,
            "[{}] {}\n",
            logLevel_str,
            fmt::format(message, std::forward<Args>(args)...));
        return;
    case LogLevel::Error:
    case LogLevel::Fatal:
        fmt::print(
            textStyle,
            "[{}] {}:{} {}\n",
            logLevel_str,
            location.file_name(),
            location.line(),
            fmt::format(message, std::forward<Args>(args)...));
        return;
    default:
        assert(false && "Unknown LogLevel");
    }
}
}

#ifndef NDEBUG
#define LOG_TRACE(message, ...)                                                \
    log(Zeus::LogLevel::Trace,                                                 \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)

#define LOG_DEBUG(message, ...)                                                \
    log(Zeus::LogLevel::Debug,                                                 \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)

#define LOG_INFO(message, ...)                                                 \
    log(Zeus::LogLevel::Info,                                                  \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)

#define LOG_WARNING(message, ...)                                              \
    log(Zeus::LogLevel::Warning,                                               \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)

#define LOG_ERROR(message, ...)                                                \
    log(Zeus::LogLevel::Error,                                                 \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)

#define LOG_FATAL(message, ...)                                                \
    log(Zeus::LogLevel::Fatal,                                                 \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)
#else
#define LOG_TRACE(message, ...)
#define LOG_DEBUG(message, ...)
#define LOG_INFO(message, ...)
#define LOG_WARNING(message, ...)
#define LOG_ERROR(message, ...)
#define LOG_FATAL(message, ...)
#endif
