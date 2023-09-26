using Editor.ViewModels.WorldEditor;

namespace Editor.ViewModels.Components;

public class ComponentViewModel : ViewModelBase
{
    public ComponentViewModel(GameEntityViewModel owner)
    {
        Owner = owner;
    }

    public GameEntityViewModel Owner { get; set; }
}