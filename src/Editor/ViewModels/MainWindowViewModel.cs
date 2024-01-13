using Editor.Models.Logging;
using Editor.ViewModels.StartPage;
using Editor.ViewModels.WorldEditor;
using ReactiveUI;
using System.Reactive.Linq;
using System.Windows.Input;

namespace Editor.ViewModels;

public class MainWindowViewModel : ViewModelBase
{
    private WorldEditorViewModel? _worldEditorViewModel;

    public MainWindowViewModel()
    {
        ShowProjectBrowserDialog = new Interaction<StartPageViewModel, ProjectViewModel?>();

        OpenDialogCommand = ReactiveCommand.CreateFromTask(async () =>
        {
            var projectViewModel = await ShowProjectBrowserDialog.Handle(new());
            if (projectViewModel is not null) WorldEditorViewModel = new(projectViewModel);
            else Logger.LogCritical("Could not load project file");
        });
    }

    public Interaction<StartPageViewModel, ProjectViewModel?> ShowProjectBrowserDialog { get; }
    public ICommand OpenDialogCommand { get; }

    public WorldEditorViewModel? WorldEditorViewModel
    {
        get => _worldEditorViewModel;
        set => this.RaiseAndSetIfChanged(ref _worldEditorViewModel, value);
    }
}