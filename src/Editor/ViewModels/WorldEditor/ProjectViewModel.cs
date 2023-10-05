using Editor.Models.History;
using Editor.Repositories.Models;
using ReactiveUI;
using System.Collections.ObjectModel;
using System.Reactive;
using System.Reactive.Linq;
using System.Windows.Input;

namespace Editor.ViewModels.WorldEditor;

public class ProjectViewModel : ViewModelBase
{
    public ProjectViewModel(ProjectFile projectFile)
    {
        Name = projectFile.Name;
        Path = projectFile.Path;

        Undo = ReactiveCommand.Create(History.Undo);
        Redo = ReactiveCommand.Create(History.Redo);

        var scenes = projectFile.Scenes.Select(scene => new SceneViewModel(this, scene.Name));
        Scenes = new ObservableCollection<SceneViewModel>(scenes);
        ActiveScene = scenes.FirstOrDefault(scene => scene.Name == projectFile.ActiveScene?.Name);

        AddScene = ReactiveCommand.Create(() =>
        {
            Scenes.Add(new SceneViewModel(this, "Test " + Scenes.Count));
            var newScene = Scenes.Last();

            ActiveScene = newScene;
            History.Add(new HistoryCommand($"Add {newScene.Name}",
                () => Scenes.Remove(newScene),
                () => Scenes.Add(newScene)));
        }, Observable.Return(true));

        RemoveScene = ReactiveCommand.Create<SceneViewModel, Unit>(scene =>
        {
            Scenes.Remove(scene);

            History.Add(new HistoryCommand($"Remove {scene.Name}",
                () => Scenes.Add(scene),
                () => Scenes.Remove(scene))); 

            return Unit.Default;
        }, Observable.Return(true));
    }

    public string Name { get; set; }
    public string Path { get; set; }

    //public static GameProject Current => Application.Current.
    // unload projects
    // loading project and saing project which means get from json file and save to json file

    public ObservableCollection<SceneViewModel> Scenes { get; set; }

    public SceneViewModel? _ActiveScene;

    public SceneViewModel? ActiveScene
    {
        get => _ActiveScene;
        set => this.RaiseAndSetIfChanged(ref _ActiveScene, value);
    }

    public ICommand Undo { get; }
    public ICommand Redo { get; }

    public ICommand AddScene { get; set; }
    public ReactiveCommand<SceneViewModel, Unit> RemoveScene { get; set; }
}