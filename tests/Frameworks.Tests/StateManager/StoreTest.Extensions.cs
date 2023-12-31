using Frameworks.StateManager;
using Frameworks.Tests.StateManager.Fakes;
using Xunit;

namespace Frameworks.Tests.StateManager;

public partial class StoreTest
{
    [Fact]
    public void CreateAction_Type_Type()
    {
        // Arrange
        const string type = "type";

        // Act
        var actionCreator = Store.CreateAction(type);
        var actual = actionCreator.Invoke();

        // Assert
        Assert.IsType<Frameworks.StateManager.Action>(actual);
        Assert.Equal(type, actual.Type);
    }

    [Fact]
    public void CreateAction_TypeWithTPayload_TypeWithTPayload()
    {
        // Arrange
        const string type = "type";
        const int payload = 42;

        // Act
        var actionCreator = Store.CreateAction<int>(type);
        var actual = actionCreator.Invoke(payload);

        // Assert
        Assert.IsType<Frameworks.StateManager.Action<int>>(actual);
        Assert.Equal(type, actual.Type);
        Assert.Equal(payload, actual.Payload);
    }

    [Fact]
    public void CreateAction_TypeWithTypePayload_TypeWithPayload()
    {
        // Arrange
        const string type = "type";
        const int payload = 42;

        // Act
        var actual = Store.CreateAction(type, typeof(ActionCreator<int>));

        // Assert
        Assert.IsType<ActionCreator<int>>(actual);

        var action = ((ActionCreator<int>)actual).Invoke(payload);

        Assert.IsType<Frameworks.StateManager.Action<int>>(action);
        Assert.Equal(type, action.Type);
        Assert.Equal(payload, action.Payload);
    }

    [Fact]
    public void CombineReducers_InitialState_NotChanged()
    {
        // Arrange
        var state = new TestState();

        static object Reducer1(object state, Frameworks.StateManager.Action action)
        {
            var output = (TestState)state;
            return output with { Value1 = nameof(Reducer1) };
        }

        static object Reducer2(object state, Frameworks.StateManager.Action action)
        {
            var output = (TestState)state;
            return output with { Value1 = nameof(Reducer2) };
        }

        static object Reducer3(object state, Frameworks.StateManager.Action action)
        {
            var output = (TestState)state;
            return output with { Value2 = nameof(Reducer3) };
        }

        // Act
        var reducer = Store.CombineReducers(Reducer1, Reducer2, Reducer3);
        var actual = reducer(state, default!);

        // Assert
        Assert.IsType<TestState>(actual);

        var testState = (TestState)actual;

        Assert.Equal(nameof(Reducer2), testState.Value1);
        Assert.Equal(nameof(Reducer3), testState.Value2);
    }
}