using System.IO;

namespace Editor.Constants;

public static class FileSystem
{
    public static readonly string DefaultProjectsFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Zeus");

    public const string SystemFolderName = ".Zeus";
    public const string ContentFolderName = "Content";
    public const string GameCodeFolderName = "GameCode";

    public const string ProjectFileExtension = ".zeus";

    public const string ProjectIconFileName = "Icon.png";
    public const string ProjectScreenshotFileName = "Screenshot.png";
    public const string ProjectFileName = $"project{ProjectFileExtension}";
}