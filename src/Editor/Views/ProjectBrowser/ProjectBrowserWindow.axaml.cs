using Avalonia.ReactiveUI;
using Editor.Extensions;
using Editor.ViewModels.ProjectBrowser;
using ReactiveUI;

namespace Editor.Views.ProjectBrowser;

public partial class ProjectBrowserWindow : ReactiveWindow<ProjectBrowserWindowViewModel>
{
    public ProjectBrowserWindow()
    {
        InitializeComponent();
        this.WhenActivated(action => action(ViewModel!.CreateProjectCommand.Subscribe(Close)));
        this.WhenActivated(action => action(ViewModel!.OpenProjectCommand.Subscribe(Close)));
        this.WhenActivated(action => action(ViewModel!.ExitProjectCommand.Subscribe(_ => Close())));

        this.SetDesignDataContext<ProjectBrowserWindowViewModel>();
    }
}