using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;
using Frameworks.StateManager.Delegates;

namespace Frameworks.StateManager.Builders;

public partial class SliceReducerBuilder<TState>
{
    public SliceReducerBuilder(string name)
    {
        Name = name;
    }

    private readonly Dictionary<ActionCreator, System.Action<ReducerSwitchBuilder<TState>>> _cases = new();

    public string Name { get; }

    public SliceReducerBuilder<TState> Add(
        CaseReducer<TState> reducer,
        [CallerArgumentExpression(nameof(reducer))] string? reducerName = null)
    {
        if (reducerName is null || !ReducerNameRegex().IsMatch(reducerName))
            throw new ArgumentException($"{nameof(reducerName)} is invalid.", nameof(reducer));

        var actionCreator = new ActionCreator(GetActionType(reducerName));
        _cases.Add(actionCreator, builder => builder.AddCase(actionCreator, reducer));

        return this;
    }

    public SliceReducerBuilder<TState> Add(ActionType action, CaseReducer<TState> reducer)
    {
        var actionCreator = new ActionCreator(GetActionType(action));
        _cases.Add(actionCreator, builder => builder.AddCase(actionCreator, reducer));

        return this;
    }

    public SliceReducerBuilder<TState> Add<TPayload>(
        CaseReducer<TPayload, TState> reducer,
        [CallerArgumentExpression(nameof(reducer))] string? reducerName = null)
    {
        if (reducerName is null || !ReducerNameRegex().IsMatch(reducerName))
            throw new ArgumentException($"{nameof(reducerName)} is invalid.", nameof(reducer));

        var actionCreator = new ActionCreator<TPayload>(GetActionType(reducerName));
        _cases.Add(actionCreator, builder => builder.AddCase(actionCreator, reducer));

        return this;
    }

    public SliceReducerBuilder<TState> Add<TPayload>(string action, CaseReducer<TPayload, TState> reducer)
    {
        var actionCreator = new ActionCreator<TPayload>(GetActionType(action));
        _cases.Add(actionCreator, builder => builder.AddCase(actionCreator, reducer));

        return this;
    }

    public IReadOnlyList<ActionCreator> BuildActionCreators() =>
        _cases.Keys.ToList();

    public System.Action<ReducerSwitchBuilder<TState>> BuildReducers() =>
        builder =>
        {
            foreach (var @case in _cases.Values)
                @case(builder);
        };

    private ActionType GetActionType(string action)
    {
        var slicePrefix = $"{Name}/";
        var actionType = action.StartsWith(slicePrefix) ? action : $"{slicePrefix}{action}";
        if (_cases.Keys.Any(creator => creator.Type == actionType))
            throw new ArgumentException($"Action type {actionType} already exists.", nameof(actionType));

        return actionType;
    }

    [GeneratedRegex(@"^\w+$")] private static partial Regex ReducerNameRegex();
}