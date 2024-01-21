using System.Collections.ObjectModel;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Data;

namespace Zeus.Logging;

public static class Logger
{
    private static LogLevel _messageFilter = LogLevel.Info | LogLevel.Warning | LogLevel.Error;
    private static readonly ObservableCollection<LogEntry> _logEntries = new();

    static Logger()
    {
        FilteredEntries = new() { Source = LogEntries };
        FilteredEntries.Filter += (s, e) =>
        {
            var type = (e.Item as LogEntry).LogLevel;
            e.Accepted = _messageFilter.HasFlag(type);
        };
    }

    public static ReadOnlyObservableCollection<LogEntry> LogEntries { get; } = new(_logEntries);

    public static CollectionViewSource FilteredEntries { get; }

    public static void LogInformation(
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        Log(LogLevel.Info, message, file, caller, line);
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

    private static async void Log(
        LogLevel logLevel,
        string message,
        [CallerFilePath] string file = "",
        [CallerMemberName] string caller = "",
        [CallerLineNumber] int line = 0)
    {
        await Application.Current.Dispatcher.BeginInvoke(() =>
            _logEntries.Add(new(logLevel, message, file, caller, line)));
    }

    public static async void Clear()
    {
        await Application.Current.Dispatcher.BeginInvoke(() =>
            _logEntries.Clear());
    }

    public static void SetFilter(LogLevel mask)
    {
        _messageFilter = mask;
        FilteredEntries.View.Refresh();
    }
}