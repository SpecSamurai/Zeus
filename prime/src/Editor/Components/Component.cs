using System.Runtime.Serialization;
using Zeus.Common;

namespace Zeus.Components;

[DataContract]
public abstract class Component : ViewModelBase
{
    public Component(GameEntity owner)
    {
        Owner = owner;
    }

    [DataMember] public GameEntity Owner { get; private set; }
}