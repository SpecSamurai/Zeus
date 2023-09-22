using Avalonia.Controls;
using Editor.Extensions;
using Editor.ViewModels.ProjectBrowser;

namespace Editor.Views.ProjectBrowser;

public partial class OpenProjectView : UserControl
{
    public OpenProjectView()
    {
        InitializeComponent();

        this.SetDesignDataContext<ProjectBrowserWindowViewModel>();
    }
}