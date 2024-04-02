using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Runtime.Serialization;
using System.Windows.Input;
using Zeus.Common;
using Zeus.Components;

namespace Zeus.Projects;

[DataContract(Name = "Scene")]
public class Scene : ViewModelBase
{
    private string _name;
    private bool _isActive;
    [DataMember(Name = nameof(GameEntities))] private readonly ObservableCollection<GameEntity> _gameEntities = new();

    public Scene(string name, Project project)
    {
        Debug.Assert(project != null);
        _name = name;
        Project = project;
        OnDeserialized(new());
    }

    [DataMember] public Project Project { get; private set; }
    public ReadOnlyObservableCollection<GameEntity> GameEntities { get; private set; }
    public ICommand AddGameEntityCommand { get; private set; }
    public ICommand RemoveGameEntityCommand { get; private set; }

    [DataMember]
    public bool IsActive
    {
        get => _isActive;
        set
        {
            if (_isActive != value)
            {
                _isActive = value;
                OnPropertyChanged(nameof(IsActive));
            }
        }
    }

    [DataMember]
    public string Name
    {
        get => _name;
        set
        {
            if (_name != value)
            {
                _name = value;
                OnPropertyChanged(nameof(Name));
            }
        }
    }

    private void AddGameEntity(GameEntity entity)
    {
        Debug.Assert(!_gameEntities.Contains(entity));
        _gameEntities.Add(entity);
    }

    private void RemoveGameEntity(GameEntity entity)
    {
        Debug.Assert(_gameEntities.Contains(entity));
        _gameEntities.Remove(entity);
    }

    [OnDeserialized]
    private void OnDeserialized(StreamingContext context)
    {
        if (_gameEntities != null)
        {
            GameEntities = new(_gameEntities);
            OnPropertyChanged(nameof(GameEntities));
        }

        AddGameEntityCommand = new RelayCommand<GameEntity>(gameEntity =>
        {
            AddGameEntity(gameEntity);
            var index = _gameEntities.Count - 1;

            Project.History.Push(
                $"Add {gameEntity.Name} to {Name}",
                undo: () => RemoveGameEntity(gameEntity),
                redo: () => _gameEntities.Insert(index, gameEntity));
        });

        RemoveGameEntityCommand = new RelayCommand<GameEntity>(gameEntity =>
        {
            var index = _gameEntities.IndexOf(gameEntity);
            RemoveGameEntity(gameEntity);

            Project.History.Push(
                $"Remove {gameEntity.Name} from {Name}",
                undo: () => _gameEntities.Insert(index, gameEntity),
                redo: () => RemoveGameEntity(gameEntity));
        });
    }
}
