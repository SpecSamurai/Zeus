namespace Frameworks.StateManager;

public class ActionCreator<TPayload> : ActionCreator
{
    public ActionCreator(ActionType type) : base(type)
    {
    }

    public override Action Invoke() =>
        new Action<TPayload>(Type, default);

    public Action<TPayload> Invoke(TPayload payload) =>
        new(Type, payload);
}