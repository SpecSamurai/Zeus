namespace Editor.Repositories.Models;

public class CreatedProjectsFile
{
    public required List<CreatedProjectsFileEntry> Projects { get; init; }
}

public class CreatedProjectsFileEntry
{
    public required string Name { get; init; }
    public required string Path { get; init; }
    public required DateTime Date { get; init; }
}