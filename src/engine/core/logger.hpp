#pragma once

#include <cassert>
#include <format>
#include <iostream>
#include <source_location>
#include <sstream>
#include <utility>

namespace Zeus
{
// https://en.wikipedia.org/wiki/ANSI_escape_code
namespace ANSI
{
constexpr const char* ESC = "\x1B[";
constexpr const char* RESET_SEQUENCE = "\x1B[0m";

namespace ForegroundColors
{
constexpr const char* RED = "31m";
constexpr const char* GREEN = "32m";
constexpr const char* YELLOW = "33m";
constexpr const char* CYAN = "36m";
constexpr const char* DEFAULT = "39m";
}

namespace BackgroundColors
{
constexpr const char* RED = "41m";
}
};

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
    const std::format_string<Args...>& message,
    const std::source_location location,
    Args&&... args)
{
    using namespace ANSI;

    std::stringstream output{};

    output << ESC;

    switch (logLevel)
    {
    case LogLevel::Trace:
        output << ForegroundColors::GREEN << "[TRACE]";
        break;
    case LogLevel::Debug:
        output << ForegroundColors::CYAN << "[DEBUG]";
        break;
    case LogLevel::Info:
        output << ForegroundColors::DEFAULT << "[INFO]";
        break;
    case LogLevel::Warning:
        output << ForegroundColors::YELLOW << "[WARNING]";
        break;
    case LogLevel::Error:
        output << ForegroundColors::RED << "[ERROR]";
        break;
    case LogLevel::Fatal:
        output << BackgroundColors::RED << "[FATAL] [" << location.file_name()
               << "] [LINE:" << location.line() << "]";
        break;
    default:
        assert(false && "Unknown LogLevel");
    }

    auto formated{ std::format(message, std::forward<Args>(args)...) };
    output << ' ' << formated << RESET_SEQUENCE;

    std::cerr << output.str() << std::endl;
}

template <typename... Args>
inline void trace(const std::format_string<Args...>& message, Args&&... args)
{
    log(LogLevel::Trace,
        message,
        std::source_location::current(),
        std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(const std::format_string<Args...>& message, Args&&... args)
{
    log(LogLevel::Debug,
        message,
        std::source_location::current(),
        std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(const std::format_string<Args...>& message, Args&&... args)
{
    log(LogLevel::Info,
        message,
        std::source_location::current(),
        std::forward<Args>(args)...);
}

template <typename... Args>
inline void warning(const std::format_string<Args...>& message, Args&&... args)
{
    log(LogLevel::Warning,
        message,
        std::source_location::current(),
        std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(const std::format_string<Args...>& message, Args&&... args)
{
    log(LogLevel::Error,
        message,
        std::source_location::current(),
        std::forward<Args>(args)...);
}
}

#define fatal(message, ...)                                                    \
    log(Zeus::LogLevel::Fatal,                                                 \
        message,                                                               \
        std::source_location::current(),                                       \
        __VA_ARGS__)
