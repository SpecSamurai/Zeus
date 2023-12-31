namespace Frameworks.StateManager;

public class ActionCreator
{
    public ActionCreator(ActionType type) =>
        Type = type;

    public ActionType Type { get; }

    public virtual Action Invoke() =>
        new(Type);

    public bool Match(Action action) =>
        Type == action.Type;
}