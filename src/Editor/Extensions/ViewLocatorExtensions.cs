using Editor.Models.Logging;

namespace Editor.Extensions;

internal static class ViewLocatorExtensions
{
    internal static TView? GetView<TView>(this object? viewModel) where TView : class
    {
        var name = viewModel?
            .GetType()
            .FullName?
            .Replace("ViewModel", "View") ?? throw new ArgumentNullException(nameof(viewModel));

        var type = Type.GetType(name);
        if (type == null)
        {
            Logger.LogCritical($"{name} view not found.");
            return default;
        }

        return Activator.CreateInstance(type) as TView;
    }
}