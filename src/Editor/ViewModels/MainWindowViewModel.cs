using Editor.ViewModels.ProjectBrowser;
using ReactiveUI;
using System.Reactive.Linq;
using System.Windows.Input;

namespace Editor.ViewModels;

public class MainWindowViewModel : ViewModelBase
{
    private readonly ProjectBrowserWindowViewModel _projectBrowserWindowViewModel;

    public MainWindowViewModel(ProjectBrowserWindowViewModel projectBrowserWindowViewModel)
    {
        _projectBrowserWindowViewModel = projectBrowserWindowViewModel;
        ShowProjectBrowserDialog = new Interaction<ProjectBrowserWindowViewModel, ProjectViewModel?>();

        OpenDialogCommand = ReactiveCommand.CreateFromTask(async () =>
        {
            Project = await ShowProjectBrowserDialog.Handle(_projectBrowserWindowViewModel);
        });
    }

    public Interaction<ProjectBrowserWindowViewModel, ProjectViewModel?> ShowProjectBrowserDialog { get; }
    public ICommand OpenDialogCommand { get; }
    public ProjectViewModel? Project { get; set; }
}