using Editor.ViewModels.Components;
using System.Collections.ObjectModel;

namespace Editor.ViewModels.WorldEditor;

public class GameEntityViewModel : ViewModelBase
{
    public GameEntityViewModel(SceneViewModel scene, string name)
    {
        Scene = scene;
        Name = name;
    }

    public bool IsEnabled { get; set; }
    public string Name { get; set; }
    public SceneViewModel Scene { get; set; }
    public ObservableCollection<ComponentViewModel> Components { get; set; } = new();
}