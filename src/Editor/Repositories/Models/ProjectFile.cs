namespace Editor.Repositories.Models;

public class ProjectFile
{
    public required string Name { get; init; }
    public required string Path { get; init; }
    public required List<ProjectFileScene> Scenes { get; init; }
    public ProjectFileScene? ActiveScene { get; set; }
}

public class ProjectFileScene
{
    public required string Name { get; init; }
    public required ProjectFile Project { get; init; }
}