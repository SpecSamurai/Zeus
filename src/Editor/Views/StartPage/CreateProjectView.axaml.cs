using Avalonia.Controls;
using Avalonia.Platform.Storage;
using Avalonia.ReactiveUI;
using Editor.Constants;
using Editor.ViewModels.StartPage;
using ReactiveUI;
using System.Reactive;

namespace Editor.Views.StartPage;

public partial class CreateProjectView : ReactiveUserControl<CreateProjectViewModel>
{
    public CreateProjectView()
    {
        InitializeComponent();
        this.WhenActivated(action => action(ViewModel!.SelectFolderDialog.RegisterHandler(SelectFolderDialogAsync)));
    }

    private async Task SelectFolderDialogAsync(InteractionContext<Unit, Uri?> interaction)
    {
        var topLevel = TopLevel.GetTopLevel(this)
            ?? throw new ArgumentNullException(nameof(TopLevel));

        var folders = await topLevel.StorageProvider.OpenFolderPickerAsync(new FolderPickerOpenOptions
        {
            AllowMultiple = false,
            SuggestedStartLocation = await topLevel.StorageProvider
                .TryGetFolderFromPathAsync(Projects.DefaultProjectsFolder)
        });

        interaction.SetOutput(folders.FirstOrDefault()?.Path);
    }
}