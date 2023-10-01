using System.Collections.ObjectModel;
using Editor.Models.Files;
using ReactiveUI;

namespace Editor.ViewModels.WorldEditor;

public class ContentViewModel : ViewModelBase
{
    private HierarchicalFolder? _selectedFolder;

    public ContentViewModel(HierarchicalFolder projectFolder)
    {
        Folders = new(projectFolder.Folders);
    }

    public ObservableCollection<HierarchicalFolder> Folders { get; private set; }

    public HierarchicalFolder? SelectedFolder
    {
        get => _selectedFolder;
        set => this.RaiseAndSetIfChanged(ref _selectedFolder, value);
    }
}