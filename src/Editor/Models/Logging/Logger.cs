using DynamicData;
using System.Runtime.CompilerServices;

namespace Editor.Models.Logging;

public static class Logger
{
    private static readonly SourceList<LogEntry> SourceList;

    static Logger()
    {
        SourceList = new();
        LogEntries = SourceList.AsObservableList();
    }

    public static IObservableList<LogEntry> LogEntries { get; }

    public static void LogDebug(
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        Log(LogLevel.Debug, message, file, caller, line);
    }

    public static void LogInformation(
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        Log(LogLevel.Information, message, file, caller, line);
    }

    public static void LogWarning(
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        Log(LogLevel.Warning, message, file, caller, line);
    }

    public static void LogError(
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        Log(LogLevel.Error, message, file, caller, line);
    }

    public static void LogCritical(
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        Log(LogLevel.Critical, message, file, caller, line);
    }

    public static void Log(LogLevel logLevel,
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        SourceList.Edit(update =>
            update.Add(new(logLevel, message, file, caller, line)));
    }

    public static void Clear()
    {
        SourceList.Clear();
    }
}