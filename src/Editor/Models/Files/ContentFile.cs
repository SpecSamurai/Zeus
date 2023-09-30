using Frameworks.Serialization;

namespace Editor.Models.Files;

public class ContentFile
{
    public ContentFile(string fullPath)
    {
        FullPath = fullPath;
    }

    public string FullPath { get; private set; }

    public string Name =>
        Path.GetFileName(FullPath);

    public string Extension =>
        Path.GetExtension(FullPath);

    public void Rename(string newFileName)
    {
        var newPath = FullPath.Replace(Name, newFileName);
        File.Move(FullPath, newPath);
        FullPath = newPath;
    }

    public void Move(string destinationFolderPath)
    {
        var newFullPath = Path.Combine(destinationFolderPath, Name);
        File.Move(FullPath, newFullPath);
        FullPath = newFullPath;
    }

    public T? GetContent<T>()
    {
        return Serializer.FromFile<T>(FullPath);
    }
}