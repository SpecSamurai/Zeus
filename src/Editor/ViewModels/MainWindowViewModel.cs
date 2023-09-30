using Editor.ViewModels.ProjectBrowser;
using ReactiveUI;
using System.Reactive.Linq;
using System.Windows.Input;
using Editor.Models.Logging;
using Editor.ViewModels.WorldEditor;

namespace Editor.ViewModels;

public class MainWindowViewModel : ViewModelBase
{
    private readonly ProjectBrowserWindowViewModel _projectBrowserWindowViewModel;
    private WorldEditorViewModel? _worldEditorViewModel;

    public MainWindowViewModel(ProjectBrowserWindowViewModel projectBrowserWindowViewModel)
    {
        _projectBrowserWindowViewModel = projectBrowserWindowViewModel;
        ShowProjectBrowserDialog = new Interaction<ProjectBrowserWindowViewModel, ProjectViewModel?>();

        OpenDialogCommand = ReactiveCommand.CreateFromTask(async () =>
        {
            var projectViewModel = await ShowProjectBrowserDialog.Handle(_projectBrowserWindowViewModel);
            if (projectViewModel is not null) WorldEditorViewModel = new(projectViewModel);
            else Logger.LogCritical("Could not load project file");
        });
    }

    public Interaction<ProjectBrowserWindowViewModel, ProjectViewModel?> ShowProjectBrowserDialog { get; }
    public ICommand OpenDialogCommand { get; }

    public WorldEditorViewModel? WorldEditorViewModel
    {
        get => _worldEditorViewModel;
        set => this.RaiseAndSetIfChanged(ref _worldEditorViewModel, value);
    }
}