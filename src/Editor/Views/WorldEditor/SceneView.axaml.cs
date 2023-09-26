using Avalonia.Controls;
using Editor.Extensions;
using Editor.ViewModels.WorldEditor;

namespace Editor.Views.WorldEditor;

public partial class SceneView : UserControl
{
    public SceneView()
    {
        InitializeComponent();
        this.SetDesignDataContext<WorldEditorViewModel>();
    }
}