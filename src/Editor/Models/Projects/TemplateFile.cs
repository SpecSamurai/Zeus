namespace Editor.Models.Projects;

public class TemplateFile
{
    public required string Name { get; init; }
    public required string ProjectFile { get; init; }
    public required string[] Folders { get; init; }
}