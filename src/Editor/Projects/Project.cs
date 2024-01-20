using Zeus.Constants;
using Zeus.Serialization;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.Serialization;
using System.Windows;
using System.Windows.Input;
using Zeus.History;
using Zeus.Common;

namespace Zeus.Projects;

[DataContract(Name = "Game")]
public class Project : ViewModelBase
{
    [DataMember(Name = nameof(Scenes))] private ObservableCollection<Scene> _scenes = new();
    private Scene _activeScene;

    public static string Extension { get; } = FileSystem.ProjectFileExtension;

    public static HistoryStack History { get; } = new HistoryStack();

    [DataMember] public string Name { get; private set; } = "New Project";
    [DataMember] public string Path { get; private set; }

    public string FullPath => System.IO.Path.Combine(Path, $"{Name}{Extension}");

    public ReadOnlyObservableCollection<Scene> Scenes { get; private set; }

    public Scene ActiveScene
    {
        get => _activeScene;
        set
        {
            if (_activeScene != value)
            {
                _activeScene = value;
                OnPropertyChanged(nameof(ActiveScene));
            }
        }
    }

    public static Project Current => Application.Current.MainWindow.DataContext as Project;

    public ICommand Undo { get; private set; }
    public ICommand Redo { get; private set; }

    public ICommand AddScene { get; private set; }
    public ICommand RemoveScene { get; private set; }

    public static Project Load(string path)
    {
        Debug.Assert(File.Exists(path));
        return Serializer.FromFile<Project>(path);
    }

    public static void Save(Project project)
    {
        project.ToFile(project.FullPath);
    }

    public void Unload()
    {

    }

    private void AddSceneInternal(string sceneName)
    {
        Debug.Assert(!string.IsNullOrWhiteSpace(sceneName));
        _scenes.Add(new(sceneName, this));
    }

    private void RemoveSceneInternal(Scene scene)
    {
        Debug.Assert(_scenes.Contains(scene));
        _scenes.Remove(scene);
    }

    [OnDeserialized]
    private void OnDeserialized(StreamingContext context)
    {
        if (_scenes != null)
        {
            Scenes = new ReadOnlyObservableCollection<Scene>(_scenes);
            OnPropertyChanged(nameof(Scenes));
        }
        ActiveScene = Scenes.FirstOrDefault(x => x.IsActive);

        AddScene = new RelayCommand<object>(x =>
        {
            AddSceneInternal($"New Scene {_scenes.Count}");
            var newScene = _scenes.Last();
            var sceneIndex = _scenes.Count - 1;

            History.Push(
                $"Add {newScene.Name}",
                undo: () => RemoveSceneInternal(newScene),
                redo: () => _scenes.Insert(sceneIndex, newScene));
        });

        RemoveScene = new RelayCommand<Scene>(scene =>
        {
            var index = _scenes.IndexOf(scene);
            RemoveSceneInternal(scene);

            History.Push(
                $"Remove {scene.Name}",
                undo: () => _scenes.Insert(index, scene),
                redo: () => RemoveSceneInternal(scene));
        },
        scene => !scene.IsActive);

        Undo = new RelayCommand<object>(x => History.Undo());
        Redo = new RelayCommand<object>(x => History.Redo());
    }
}
