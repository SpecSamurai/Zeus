using Frameworks.StateManager.Delegates;

namespace Frameworks.StateManager;

public record Slice<TState>(
    string Name,
    TState InitialState,
    Reducer Reducer,
    IReadOnlyList<ActionCreator> ActionCreators);