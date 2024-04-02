using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Runtime.Serialization;
using Zeus.Common;
using Zeus.Projects;

namespace Zeus.Components;

[DataContract]
[KnownType(typeof(Transform))]
public class GameEntity : ViewModelBase
{
    private bool _isEnabled;
    private string _name;
    [DataMember(Name = nameof(Components))] private readonly ObservableCollection<Component> _components = new();

    public GameEntity(string name, Scene scene)
    {
        Debug.Assert(scene != null);
        Name = name;
        ParentScene = scene;
        _components.Add(new Transform(this));
        OnDeserialized(new());
    }

    [DataMember] public Scene ParentScene { get; private set; }
    public ReadOnlyObservableCollection<Component> Components { get; private set; }

    [DataMember]
    public bool IsEnabled
    {
        get => _isEnabled;
        set
        {
            if (_isEnabled != value)
            {
                _isEnabled = value;
                OnPropertyChanged(nameof(IsEnabled));
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

    [OnDeserialized]
    private void OnDeserialized(StreamingContext context)
    {
        if (_components != null)
        {
            Components = new(_components);
            OnPropertyChanged(nameof(Components));
        }
    }
}
