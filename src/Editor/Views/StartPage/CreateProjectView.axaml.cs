using Avalonia.ReactiveUI;
using Editor.ViewModels.StartPage;
using ReactiveUI;

namespace Editor.Views.StartPage;

public partial class CreateProjectView : ReactiveUserControl<CreateProjectViewModel>
{
    public CreateProjectView()
    {
        this.WhenActivated(disposables => { });
        InitializeComponent();
    }
}