namespace Editor.Models.Files;

public class HierarchicalFolder
{
    public HierarchicalFolder(string fullPath)
    {
        FullPath = fullPath;
    }

    public string FullPath { get; private set; }

    public string Name =>
        new DirectoryInfo(FullPath).Name;

    public List<ContentFile> Files { get; set; } = new();
    public List<HierarchicalFolder> Folders { get; set; } = new();

    public T? GetFileContent<T>(string fileName)
    {
        return Files.Single(file => file.Name == fileName).GetContent<T>();
    }
}