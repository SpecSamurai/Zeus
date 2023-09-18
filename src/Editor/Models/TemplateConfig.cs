namespace Editor.Models;

public class TemplateConfig
{
    public required string Name { get; init; }
    public required string ProjectFile { get; init; }
    public required string IconFile { get; init; }
    public required string ScreenshotFile { get; init; }
    public required string[] Folders { get; init; }
}