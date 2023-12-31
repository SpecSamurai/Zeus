using Frameworks.StateManager.Builders;
using Frameworks.StateManager.Delegates;

namespace Frameworks.StateManager;

public partial class Store
{
    public static ActionCreator CreateAction(ActionType type) => new(type);

    public static ActionCreator<TPayload> CreateAction<TPayload>(ActionType type) => new(type);

    public static ActionCreator CreateAction(ActionType type, Type actionType) =>
        Activator.CreateInstance(actionType, type) as ActionCreator
        ?? throw new InvalidOperationException($"Instance of '{actionType.FullName}' not created.");

    public static Reducer CombineReducers(params Reducer[] reducers) =>
        (state, action) => reducers.Aggregate(state, (current, reducer) => reducer(current, action));

    public static Reducer CreateReducer<TState>(
        TState initialState,
        System.Action<ReducerSwitchBuilder<TState>> builderCallback)
    {
        var builder = new ReducerSwitchBuilder<TState>();
        builderCallback(builder);
        return builder.BuildReducer(initialState);
    }
}