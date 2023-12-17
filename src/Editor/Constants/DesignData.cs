using Avalonia.Media.Imaging;
using Avalonia.Platform;
using Editor.ViewModels.Projects;

namespace Editor.Constants;

public static class DesignData
{
    public static readonly ProjectTileViewModel ProjectTileViewModel = new()
    {
        Image = new Bitmap(AssetLoader.Open(new Uri("avares://Editor/Assets/DesignTime/Placeholder100x100.png"))),
        Name = nameof(ProjectTileViewModel.Name)
    };
}