namespace Editor.Repositories.Models;

public class TemplateFile
{
    public required string Name { get; init; }
    public required string[] Folders { get; init; }
}