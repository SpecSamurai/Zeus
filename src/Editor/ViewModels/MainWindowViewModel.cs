using Editor.ViewModels.ProjectBrowser;
using ReactiveUI;
using System.Reactive.Linq;
using System.Windows.Input;

namespace Editor.ViewModels;

public class MainWindowViewModel : ViewModelBase
{
    public MainWindowViewModel()
    {
        ShowProjectBrowserDialog = new Interaction<ProjectBrowserWindowViewModel, ProjectViewModel?>();

        OpenDialogCommand = ReactiveCommand.CreateFromTask(async () =>
        {
            var model = new ProjectBrowserWindowViewModel();
            Project = await ShowProjectBrowserDialog.Handle(model);
        });
    }

    public Interaction<ProjectBrowserWindowViewModel, ProjectViewModel?> ShowProjectBrowserDialog { get; }
    public ICommand OpenDialogCommand { get; }
    public ProjectViewModel? Project { get; set; }
}