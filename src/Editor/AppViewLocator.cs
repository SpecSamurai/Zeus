using Editor.Extensions;
using ReactiveUI;

namespace Editor;

public class AppViewLocator : IViewLocator
{
    public IViewFor? ResolveView<T>(T? viewModel, string? contract = null)
    {
        return viewModel.GetView<IViewFor>();
    }
}