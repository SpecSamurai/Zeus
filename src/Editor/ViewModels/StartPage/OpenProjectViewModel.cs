using Editor.ViewModels.WorldEditor;
using Editor.Views.StartPage;
using ReactiveUI;

namespace Editor.ViewModels.StartPage;

public class OpenProjectViewModel : ViewModelBase, IRoutableViewModel
{
    public OpenProjectViewModel(
        IScreen screen,
        IReadOnlyList<ProjectTileViewModel> projects,
        ReactiveCommand<ProjectTileViewModel, ProjectViewModel> openProjectCommand)
    {
        HostScreen = screen;
        Projects = projects;
        OpenProjectCommand = openProjectCommand;
    }

    private ProjectTileViewModel? _selectedProject;

    public IScreen HostScreen { get; }
    public string? UrlPathSegment { get; } = nameof(OpenProjectView);
    public IReadOnlyList<ProjectTileViewModel> Projects { get; }
    public ReactiveCommand<ProjectTileViewModel, ProjectViewModel> OpenProjectCommand { get; }

    public ProjectTileViewModel? SelectedProject
    {
        get => _selectedProject;
        set => this.RaiseAndSetIfChanged(ref _selectedProject, value);
    }
}