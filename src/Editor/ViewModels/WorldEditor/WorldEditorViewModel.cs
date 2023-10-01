using Editor.Services;
using ReactiveUI;

namespace Editor.ViewModels.WorldEditor;

public class WorldEditorViewModel : ViewModelBase
{
    private ProjectViewModel _projectViewModel;
    private GameEntityViewModel? _activeGameEntity;
    private ContentViewModel? _contentViewModel;

    public WorldEditorViewModel(ProjectViewModel projectViewModel)
    {
        _projectViewModel = projectViewModel;
        var folder = FileManager.GetFolder(_projectViewModel.Path);
        ContentViewModel = new(folder);
    }

    public ContentViewModel? ContentViewModel
    {
        get => _contentViewModel;
        set => this.RaiseAndSetIfChanged(ref _contentViewModel, value);
    }

    public GameEntityViewModel? ActiveGameEntity
    {
        get => _activeGameEntity;
        set => this.RaiseAndSetIfChanged(ref _activeGameEntity, value);
    }

    public ProjectViewModel ProjectViewModel
    {
        get => _projectViewModel;
        set => this.RaiseAndSetIfChanged(ref _projectViewModel, value);
    }
}