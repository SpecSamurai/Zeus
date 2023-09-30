using System.Numerics;
using Editor.ViewModels.WorldEditor;

namespace Editor.ViewModels.Components;

public class TransformViewModel : ComponentViewModel
{
    public TransformViewModel(GameEntityViewModel owner)
        : base(owner)
    {
    }

    public Vector3 Position { get; set; }
    public Vector3 Rotation { get; set; }
    public Vector3 Scale { get; set; }
}