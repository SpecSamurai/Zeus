using System.Diagnostics;
using System.Runtime.Serialization;
using Zeus.Common;

namespace Zeus.Projects;

[DataContract(Name = "Scene")]
public class Scene : ViewModelBase
{
    private string _name;
    private bool _isActive;

    public Scene(string name, Project project)
    {
        Debug.Assert(project != null);
        _name = name;
        Project = project;
    }

    [DataMember] public Project Project { get; private set; }

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
}
