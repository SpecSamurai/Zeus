using Frameworks.StateManager.Delegates;

namespace Frameworks.StateManager.Builders;

public class ReducerSwitchBuilder<TState>
{
    private readonly Dictionary<string, Delegate> _cases = new();
    private readonly Dictionary<ActionMatcher, Delegate> _matchers = new();
    private Delegate? _default;

    public ReducerSwitchBuilder<TState> AddCase(ActionCreator action, CaseReducer<TState> reducer)
    {
        _cases.Add(action.Type, reducer);
        return this;
    }

    public ReducerSwitchBuilder<TState> AddCase<TPayload>(
        ActionCreator<TPayload> action,
        CaseReducer<TPayload, TState> reducer)
    {
        _cases.Add(action.Type, reducer);
        return this;
    }

    public ReducerSwitchBuilder<TState> AddDefaultCase(CaseReducer<TState> reducer)
    {
        _default = reducer;
        return this;
    }

    public ReducerSwitchBuilder<TState> AddDefaultCase<TPayload>(CaseReducer<TPayload, TState> reducer)
    {
        _default = reducer;
        return this;
    }

    public ReducerSwitchBuilder<TState> AddMatcher(ActionMatcher actionMatcher, CaseReducer<TState> reducer)
    {
        _matchers.Add(actionMatcher, reducer);
        return this;
    }

    public ReducerSwitchBuilder<TState> AddMatcher<TPayload>(
        ActionMatcher actionMatcher,
        CaseReducer<TPayload, TState> reducer)
    {
        _matchers.Add(actionMatcher, reducer);
        return this;
    }

    public Reducer BuildReducer(TState initialState)
    {
        ArgumentNullException.ThrowIfNull(initialState);

        return (state, action) =>
        {
            var isStateModified = false;
            state ??= initialState;

            if (_cases.TryGetValue(action.Type, out var caseReducer))
            {
                state = caseReducer.Reduce(state, action);
                isStateModified = true;
            }

            foreach (var (actionMatcher, reducer) in _matchers)
                if (actionMatcher(action))
                {
                    state = reducer.Reduce(state, action);
                    isStateModified = true;
                }

            if (!isStateModified && _default != null)
                state = _default.Reduce(state, action);

            return state;
        };
    }
}

file static class ReducerExtensions
{
    internal static object Reduce(this Delegate reducer, object state, Action action) =>
        reducer.DynamicInvoke(state, action)
        ?? throw new NullReferenceException("State cannot be null.");
}