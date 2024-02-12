using Zeus.Common;

namespace Zeus.Components;

public abstract class MSComponent<T> : ViewModelBase, IMSComponent where T : Component
{
}