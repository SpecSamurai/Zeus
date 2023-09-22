using Avalonia.Media.Imaging;
using Editor.Models.Projects;

namespace Editor.ViewModels.ProjectBrowser;

public class CreatedProjectViewModel
{
    public CreatedProjectViewModel(CreatedProject data)
    {
        Name = data.Name;
        ProjectFilePath = data.ProjectFilePath;
        Icon = new Bitmap(data.IconFilePath);
        Screenshot = new Bitmap(data.ScreenshotFilePath);
    }

    public string Name { get; }
    public string ProjectFilePath { get; }
    public Bitmap Icon { get; }
    public Bitmap Screenshot { get; }

    public static CreatedProjectViewModel Create(CreatedProject data) =>
        new(data);
}