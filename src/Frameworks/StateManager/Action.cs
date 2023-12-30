namespace Frameworks.StateManager;

public record Action(string Type);
public record Action<TPayload>(string Type, TPayload? Payload) : Action(Type);