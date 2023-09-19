namespace Editor.Models;

public partial class ProjectTemplate
{
    public required string IconFile { get; init; }
    public required string ScreenshotFile { get; init; }
    public required string ProjectFile { get; init; }
    public required string TemplateDirectoryPath { get; init; }
    public required string[] Folders { get; init; }

    public string IconFilePath => Path.Combine(TemplateDirectoryPath, IconFile);
    public string ScreenshotFilePath => Path.Combine(TemplateDirectoryPath, ScreenshotFile);
    public string ProjectFilePath => Path.Combine(TemplateDirectoryPath, ProjectFile);
}