using Avalonia.Controls;
using Avalonia.Controls.Templates;
using Editor.Extensions;
using Editor.ViewModels;

namespace Editor;

public class ViewLocator : IDataTemplate
{
    public Control? Build(object? viewModel)
    {
        return viewModel.GetView<Control>() ?? new TextBlock { Text = "View model not Found." };
    }

    public bool Match(object? data)
    {
        return data is ViewModelBase;
    }
}