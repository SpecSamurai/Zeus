namespace Zeus.Logging;

[Flags]
public enum LogLevel
{
    Info = 0x01,
    Warning = 0x02,
    Error = 0x04,
}