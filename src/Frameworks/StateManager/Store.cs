using Frameworks.StateManager.Builders;
using Frameworks.StateManager.Delegates;

namespace Frameworks.StateManager;

public partial class Store
{
    private readonly Dictionary<string, object> _state = new();
    private readonly Dictionary<string, Reducer> _reducers = new();
    private readonly Dictionary<string, List<System.Action<object>>> _subscribers = new();

    public IReadOnlyDictionary<string, object> State => _state;

    public TState GetState<TState>(string name) =>
        (TState)_state[name];

    public Slice<TState> CreateSlice<TState>(
        string name,
        TState initialState,
        System.Action<SliceReducerBuilder<TState>> reducers,
        System.Action<ReducerSwitchBuilder<TState>>? extraReducers = null)
    {
        ArgumentNullException.ThrowIfNull(initialState);
        if (_state.ContainsKey(name))
            throw new ArgumentException($"State slice '{name}' already exists.");

        var builder = new SliceReducerBuilder<TState>(name);
        reducers(builder);

        var sliceReducer = CreateReducer(initialState, builder.BuildReducers());
        _reducers[name] = extraReducers == null
            ? sliceReducer
            : CombineReducers(sliceReducer, CreateReducer(initialState, extraReducers));

        _state[name] = initialState;

        return new(name, initialState, sliceReducer, builder.BuildActionCreators());
    }

    public Unsubscriber Subscribe<TState>(string sliceName, System.Action<TState> listener)
    {
        if (!_subscribers.ContainsKey(sliceName))
            _subscribers[sliceName] = new();

        System.Action<object> invokeListener = state => listener.Invoke((TState)state);
        _subscribers[sliceName].Add(invokeListener);

        return () => _subscribers[sliceName].Remove(invokeListener);
    }

    public void NotifyStateChanged(string name)
    {
        if (_state.TryGetValue(name, out var state) && _subscribers.TryGetValue(name, out var subscribers))
            foreach (var subscriber in subscribers)
                subscriber.Invoke(state);
    }

    public void Dispatch(ActionType actionType) =>
        Dispatch(CreateAction(actionType).Invoke());

    public void Dispatch<TPayload>(ActionType actionType, TPayload payload) =>
        Dispatch(CreateAction<TPayload>(actionType).Invoke(payload));

    public void Dispatch(Action action)
    {
        if (action.Type.IsSlice)
        {
            DispatchSlice(action);
            return;
        }

        foreach (var slice in _reducers.Keys)
        {
            _state[slice] = _reducers[slice](_state[slice], action);

            if (_subscribers.TryGetValue(slice, out var subscribers))
            {
                foreach (var subscriber in subscribers)
                    subscriber.Invoke(_state[slice]);
            }
        }
    }

    private void DispatchSlice(Action action)
    {
        var slice = action.Type.SlicePrefix;

        if (!_reducers.TryGetValue(slice, out var reducer))
            throw new ArgumentException($"Reducer for action type '{action.Type}' does not exist.");

        _state[slice] = reducer(_state[slice], action);

        if (_subscribers.TryGetValue(slice, out var subscribers))
        {
            foreach (var subscriber in subscribers)
                subscriber.Invoke(_state[slice]);
        }
    }
}