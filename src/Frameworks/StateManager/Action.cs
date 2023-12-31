namespace Frameworks.StateManager;

public record Action(ActionType Type);
public record Action<TPayload>(ActionType Type, TPayload? Payload) : Action(Type);