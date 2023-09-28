namespace Editor.Models.Logging;

[Flags]
public enum LogLevel
{
    Trace = 0,
    Debug = 1,
    Information = 2,
    Warning = 4,
    Error = 8,
    Critical = 16,
    None = 32
}