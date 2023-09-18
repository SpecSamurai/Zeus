using Avalonia.Controls;
using Avalonia.Controls.Templates;
using Editor.ViewModels;

namespace Editor;

public class ViewLocator : IDataTemplate
{
    public Control Build(object? data)
    {
        var name = data?.GetType().FullName?.Replace("ViewModel", "View")
            ?? throw new ArgumentNullException(nameof(data));

        var type = Type.GetType(name);

        return type != null
            ? (Control)Activator.CreateInstance(type)!
            : new TextBlock { Text = "Not Found: " + name };
    }

    public bool Match(object? data)
    {
        return data is ViewModelBase;
    }
}