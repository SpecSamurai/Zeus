namespace Editor.Models.Projects;

public class Template
{
    public required string Name { get; init; }
    public required string[] Folders { get; init; }
    public required string IconFilePath { get; init; }
    public required string ScreenshotFilePath { get; init; }
    public required string ProjectFilePath { get; init; }
}