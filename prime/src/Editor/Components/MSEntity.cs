using System.Collections.ObjectModel;
using System.Diagnostics;
using Zeus.Common;

namespace Zeus.Components;

public abstract class MSEntity : ViewModelBase
{
    private bool _enableUpdates = true;
    private bool? _isEnabled;
    private string _name;
    private readonly ObservableCollection<IMSComponent> _components = new();

    public MSEntity(List<GameEntity> entities)
    {
        Debug.Assert(entities?.Any() == true);
        Components = new(_components);
        SelectedEntities = entities;
        PropertyChanged += (s, e) =>
        {
            if(_enableUpdates)
            {
                UpdateGameEntities(e.PropertyName);
            }
        };
    }

    public ReadOnlyObservableCollection<IMSComponent> Components { get; }

    public List<GameEntity> SelectedEntities { get; }

    public void Refresh()
    {
        _enableUpdates = false;
        UpdateMSGameEntities();
        _enableUpdates = true;
    }

    public bool? IsEnabled
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

    public static float? GetMixedValue(List<GameEntity> selectedEntities, Func<GameEntity, float> getProperty)
    {
        var value = getProperty(selectedEntities.First());
        foreach (var entity in selectedEntities.Skip(1))
        {
            if (!value.IsTheSameAs(getProperty(entity)))
            {
                return null;
            }
        }

        return value;
    }

    public static bool? GetMixedValue(List<GameEntity> selectedEntities, Func<GameEntity, bool> getProperty)
    {
        var value = getProperty(selectedEntities.First());
        foreach (var entity in selectedEntities.Skip(1))
        {
            if (value != getProperty(entity))
            {
                return null;
            }
        }

        return value;
    }

    public static string GetMixedValue(List<GameEntity> selectedEntities, Func<GameEntity, string> getProperty)
    {
        var value = getProperty(selectedEntities.First());
        foreach (var entity in selectedEntities.Skip(1))
        {
            if (value != getProperty(entity))
            {
                return null;
            }
        }

        return value;
    }

    protected virtual bool UpdateGameEntities(string propertyName)
    {
        switch (propertyName)
        {
            case nameof(IsEnabled):
                SelectedEntities.ForEach(x => x.IsEnabled = IsEnabled.Value);
                return true;
            case nameof(Name):
                SelectedEntities.ForEach(x => x.Name = Name);
                return true;
        }

        return false;
    }

    protected virtual bool UpdateMSGameEntities()
    {
        IsEnabled = GetMixedValue(SelectedEntities, new Func<GameEntity, bool>(x => x.IsEnabled));
        Name = GetMixedValue(SelectedEntities, new Func<GameEntity, string>(x => x.Name));

        return true;
    }
}
