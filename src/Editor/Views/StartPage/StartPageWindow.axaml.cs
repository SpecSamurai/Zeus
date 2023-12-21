using Avalonia.ReactiveUI;
using Editor.ViewModels.StartPage;
using ReactiveUI;

namespace Editor.Views.StartPage;

public partial class StartPageWindow : ReactiveWindow<StartPageViewModel>
{
    public StartPageWindow()
    {
        this.WhenActivated(disposables => { });
        InitializeComponent();
    }
}