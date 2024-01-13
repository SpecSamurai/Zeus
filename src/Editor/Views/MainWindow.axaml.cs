using Avalonia.Interactivity;
using Avalonia.ReactiveUI;
using Editor.ViewModels;
using Editor.ViewModels.StartPage;
using Editor.ViewModels.WorldEditor;
using Editor.Views.StartPage;
using ReactiveUI;
using System.Reactive;

namespace Editor.Views;

public partial class MainWindow : ReactiveWindow<MainWindowViewModel>
{
    public MainWindow()
    {
        InitializeComponent();
        this.WhenActivated(action => action(ViewModel!.ShowProjectBrowserDialog.RegisterHandler(ShowStartPageDialogAsync)));
        Loaded += ShowProjectBrowserDialogOnLoaded;
    }

    private async Task ShowStartPageDialogAsync(InteractionContext<StartPageViewModel, ProjectViewModel?> interaction)
    {
        var dialog = new StartPageWindow
        {
            DataContext = interaction.Input
        };

        var result = await dialog.ShowDialog<ProjectViewModel?>(this);
        interaction.SetOutput(result);
    }

    private void ShowProjectBrowserDialogOnLoaded(object? sender, RoutedEventArgs e)
    {
        ViewModel!.OpenDialogCommand.Execute(Unit.Default);
    }
}