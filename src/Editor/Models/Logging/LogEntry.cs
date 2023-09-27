namespace Editor.Models.Logging;

public class LogEntry
{
    public LogEntry(LogLevel logLevel, string message, string file, string caller, int line)
    {
        LogLevel = logLevel;
        Message = message;
        File = file;
        Caller = caller;
        Line = line;
    }

    public LogLevel LogLevel { get; }
    public string Message { get; }
    public string File { get; }
    public string Caller { get; }
    public int Line { get; }
}