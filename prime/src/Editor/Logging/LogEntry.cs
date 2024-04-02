using System.IO;

namespace Zeus.Logging;

public class LogEntry
{
    public LogEntry(LogLevel logLevel, string message, string file, string caller, int line)
    {
        DateTime = DateTime.Now;
        LogLevel = logLevel;
        Message = message;
        File = Path.GetFileName(file);
        Caller = caller;
        Line = line;
    }

    public DateTime DateTime { get; }
    public LogLevel LogLevel { get; }
    public string Message { get; }
    public string File { get; }
    public string Caller { get; }
    public int Line { get; }
    public string MetaData => $"{File}: {Caller} ({Line})";
}