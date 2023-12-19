using Avalonia.Media.Imaging;

namespace Editor.ViewModels.StartPage;

public class ProjectTileViewModel : ViewModelBase
{
    public required Bitmap Image { get; init; }
    public required string Name { get; init; }
}