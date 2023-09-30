using Editor.Models.Files;
using Frameworks;

namespace Editor.Services;

public static class FileManager
{
    public static HierarchicalFolder GetFolder(string path) =>
        Functional.NavigateHierarchically(
            path,
            Directory.GetDirectories,
            (folderPath, children) => new HierarchicalFolder(folderPath)
            {
                Folders = children.ToList(),
                Files = GetFiles(folderPath)
            },
            folderPath => new HierarchicalFolder(folderPath)
            {
                Files = GetFiles(folderPath)
            });

    private static List<ContentFile> GetFiles(string folderPath) =>
        Directory
            .GetFiles(folderPath)
            .Select(filePath => new ContentFile(filePath))
            .ToList();
}