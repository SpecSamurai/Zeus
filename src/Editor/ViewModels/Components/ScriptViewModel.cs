using Editor.ViewModels.WorldEditor;

namespace Editor.ViewModels.Components;

public class ScriptViewModel : ComponentViewModel
{
    public ScriptViewModel(GameEntityViewModel owner)
        : base(owner)
    {
    }

    public string? Script { get; set; }
}