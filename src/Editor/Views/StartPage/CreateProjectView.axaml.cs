using Avalonia.ReactiveUI;
using Editor.ViewModels.StartPage;

namespace Editor.Views.StartPage;

public partial class CreateProjectView : ReactiveUserControl<CreateProjectViewModel>
{
    public CreateProjectView()
    {
        InitializeComponent();
    }
}