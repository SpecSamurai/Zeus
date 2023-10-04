using Editor.Models.History;
using Editor.Models.Logging;
using ReactiveUI;
using System.Collections.ObjectModel;
using System.Reactive;
using System.Reactive.Linq;
using System.Windows.Input;
using Editor.ViewModels.Components;

namespace Editor.ViewModels.WorldEditor;

public class SceneViewModel : ViewModelBase
{
    public SceneViewModel(ProjectViewModel project, string name)
    {
        Project = project;
        Name = name;

        AddGameEntity = ReactiveCommand.Create(() =>
        {
            var ge = new GameEntityViewModel(this, "Game ENtity" + GameEntities.Count);
            ge.Components.Add(new TransformViewModel(ge));
            ge.Components.Add(new ScriptViewModel(ge) { Script = "script path" });
            GameEntities.Add(ge);
            var newScene = GameEntities.Last();
            History.Add(new HistoryCommand($"Add {newScene.Name}",
                () => GameEntities.Remove(newScene),
                () => GameEntities.Add(newScene)));

            Logger.LogInformation($"Add new entity: {newScene.Name}.");
            Logger.LogError($"Add new entity: {newScene.Name}.");
            Logger.LogWarning($"Add new entity: {newScene.Name}.");
            Logger.LogDebug($"Add new entity: {newScene.Name}.");
            Logger.LogCritical($"Add new entity: {newScene.Name}.");
        }, Observable.Return(true));

        RemoveGameEntity = ReactiveCommand.Create<GameEntityViewModel, Unit>(scene =>
        {
            GameEntities.Remove(scene);

            History.Add(new HistoryCommand($"Remove {scene.Name}",
                () => GameEntities.Add(scene),
                () => GameEntities.Remove(scene)));

            return Unit.Default;
        }, Observable.Return(true));

        SelectGameEntity = ReactiveCommand.Create<GameEntityViewModel, Unit>(scene =>
        {
            ActiveGameEntity = scene;
            return Unit.Default;
        }, Observable.Return(true));
    }

    public string Name { get; set; }
    public ProjectViewModel Project { get; set; }
    public ObservableCollection<GameEntityViewModel> GameEntities { get; set; } = new ObservableCollection<GameEntityViewModel>();

    public ICommand AddGameEntity { get; set; }
    public ReactiveCommand<GameEntityViewModel, Unit> RemoveGameEntity { get; set; }
    public ReactiveCommand<GameEntityViewModel, Unit> SelectGameEntity { get; set; }

    public bool IsActive => Project.ActiveScene == this;
    
    public GameEntityViewModel _ActiveGameEntity;
    public GameEntityViewModel ActiveGameEntity
    {
        get => _ActiveGameEntity;
        set => this.RaiseAndSetIfChanged(ref _ActiveGameEntity, value);
    }
}