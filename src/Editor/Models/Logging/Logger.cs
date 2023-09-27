using Avalonia.Threading;
using System.Collections.ObjectModel;
using System.Runtime.CompilerServices;

namespace Editor.Models.Logging;

public static class Logger
{
    // implement filtering
    public static ObservableCollection<LogEntry> LogEntries { get; }

    static Logger()
    {
        LogEntries = new ObservableCollection<LogEntry>();
    }

    public static async void LogDebug(string message, [CallerFilePath] string file = "", [CallerMemberName] string caller = "", [CallerLineNumber] int line = 0)
    {
        await Log(LogLevel.Debug, message, file, caller, line);
    }

    public static async void LogInformation(string message, [CallerFilePath] string file = "", [CallerMemberName] string caller = "", [CallerLineNumber] int line = 0)
    {
        await Log(LogLevel.Information, message, file, caller, line);
    }

    public static async void LogWarning(string message, [CallerFilePath] string file = "", [CallerMemberName] string caller = "", [CallerLineNumber] int line = 0)
    {
        await Log(LogLevel.Warning, message, file, caller, line);
    }

    public static async void LogError(string message, [CallerFilePath] string file = "", [CallerMemberName] string caller = "", [CallerLineNumber] int line = 0)
    {
        await Log(LogLevel.Error, message, file, caller, line);
    }

    public static async void LogCrititcal(string message, [CallerFilePath] string file = "", [CallerMemberName] string caller = "", [CallerLineNumber] int line = 0)
    {
        await Log(LogLevel.Critical, message, file, caller, line);
    }

    public static async Task Log(LogLevel logLevel, string message, [CallerFilePath] string file = "", [CallerMemberName] string caller = "", [CallerLineNumber] int line = 0)
    {
        await Dispatcher.UIThread.InvokeAsync(() =>
        {
            LogEntries.Add(new(logLevel, message, file, caller, line));
        });
    }

    public static async Task Clear()
    {
        await Dispatcher.UIThread.InvokeAsync(LogEntries.Clear);
    }
}