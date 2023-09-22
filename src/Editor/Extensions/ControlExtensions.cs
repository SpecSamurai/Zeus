using Avalonia.Controls;
using DryIoc;

namespace Editor.Extensions;

public static class ControlExtensions
{
    public static void SetDesignDataContext<T>(this Control control)
    {
        if (Design.IsDesignMode && Design.GetDataContext(control) == null)
            Design.SetDataContext(control, ServiceLocator.Container.Resolve<T>()!);
    }
}