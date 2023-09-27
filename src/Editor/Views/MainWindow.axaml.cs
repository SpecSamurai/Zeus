using Avalonia.Interactivity;
using Avalonia.ReactiveUI;
using Editor.Extensions;
using Editor.ViewModels;
using Editor.ViewModels.ProjectBrowser;
using Editor.ViewModels.WorldEditor;
using Editor.Views.ProjectBrowser;
using ReactiveUI;
using System.Reactive;

namespace Editor.Views;

public partial class MainWindow : ReactiveWindow<MainWindowViewModel>
{
    public MainWindow()
    {
        InitializeComponent();
        this.WhenActivated(action => action(ViewModel!.ShowProjectBrowserDialog.RegisterHandler(ShowProjectBrowserDialogAsync)));
        Loaded += ShowProjectBrowserDialogOnLoaded;

        this.SetDesignDataContext<MainWindowViewModel>();
    }

    private async Task ShowProjectBrowserDialogAsync(InteractionContext<ProjectBrowserWindowViewModel, ProjectViewModel?> interaction)
    {
        var dialog = new ProjectBrowserWindow();
        dialog.DataContext = interaction.Input;

        var result = await dialog.ShowDialog<ProjectViewModel?>(this);
        interaction.SetOutput(result);
    }

    private void ShowProjectBrowserDialogOnLoaded(object? sender, RoutedEventArgs e)
    {
        ViewModel!.OpenDialogCommand.Execute(Unit.Default);
    }
}