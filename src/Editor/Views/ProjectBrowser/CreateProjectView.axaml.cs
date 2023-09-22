using Avalonia.Controls;
using Editor.Extensions;
using Editor.ViewModels.ProjectBrowser;

namespace Editor.Views.ProjectBrowser;

public partial class CreateProjectView : UserControl
{
    public CreateProjectView()
    {
        InitializeComponent();
        this.SetDesignDataContext<ProjectBrowserWindowViewModel>();
    }
}