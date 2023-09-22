using Avalonia.Media.Imaging;
using Editor.Models.Projects;

namespace Editor.ViewModels.ProjectBrowser;

public class TemplateViewModel
{
    public TemplateViewModel(Template template)
    {
        Template = template;
        Name = template.Name;
        Icon = new Bitmap(template.IconFilePath);
        Screenshot = new Bitmap(template.ScreenshotFilePath);
    }

    public Template Template { get; }
    public string Name { get; }
    public Bitmap Icon { get; }
    public Bitmap Screenshot { get; }

    public static TemplateViewModel Create(Template template) =>
        new(template);
}