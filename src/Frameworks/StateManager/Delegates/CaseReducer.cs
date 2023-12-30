namespace Frameworks.StateManager.Delegates;

public delegate TState CaseReducer<TState>(TState state, Action action);

public delegate TState CaseReducer<TPayload, TState>(TState state, Action<TPayload> action);