using Avalonia.ReactiveUI;
using Editor.ViewModels.StartPage;
using ReactiveUI;

namespace Editor.Views.StartPage;

public partial class StartPageWindow : ReactiveWindow<StartPageViewModel>
{
    public StartPageWindow()
    {
        InitializeComponent();
        this.WhenActivated(action => action(ViewModel!.CreateProjectCommand.Subscribe(Close)));
        this.WhenActivated(action => action(ViewModel!.OpenProjectCommand.Subscribe(Close)));
    }
}