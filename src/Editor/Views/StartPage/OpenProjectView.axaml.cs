using Avalonia.ReactiveUI;
using Editor.ViewModels.StartPage;
using ReactiveUI;

namespace Editor.Views.StartPage;

public partial class OpenProjectView : ReactiveUserControl<OpenProjectViewModel>
{
    public OpenProjectView()
    {
        this.WhenActivated(disposables => { });
        InitializeComponent();
    }
}