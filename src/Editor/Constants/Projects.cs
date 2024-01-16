namespace Editor.Constants;

public static class Projects
{
    public static readonly string DefaultProjectsFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Zeus");

    public const string ProjectFileExtension = ".zeus";

    public const string SystemFolderName = ".Zeus";
    public const string ProjectIconFileName = "Icon.png";
    public const string ProjectScreenshotFileName = "Screenshot.png";
    public const string ProjectFileName = $"project{ProjectFileExtension}";
}