using Avalonia.ReactiveUI;
using Editor.ViewModels.StartPage;

namespace Editor.Views.StartPage;

public partial class OpenProjectView : ReactiveUserControl<OpenProjectViewModel>
{
    public OpenProjectView()
    {
        InitializeComponent();
    }
}