using Frameworks.StateManager;
using Frameworks.StateManager.Builders;
using Frameworks.Tests.StateManager.Fakes;
using Xunit;
using Action = Frameworks.StateManager.Action;

namespace Frameworks.Tests.StateManager.Builders;

public class ReducerSwitchBuilderTest
{
    [Fact]
    public void BuildReducer_Empty_StateNotModified()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var testAction = new Action("Test");

        //Act
        var reducer = sut.BuildReducer(initialState);
        var actual = reducer(initialState, testAction);

        //Assert
        Assert.Equal(initialState, actual);
    }

    [Fact]
    public void AddCase_AddCaseTwiceForTheSameType_ThrowsArgumentException()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var actionCreator1 = new ActionCreator("Type");
        var actionCreator2 = new ActionCreator("Type");

        //Act
        void AddCases() => sut
           .AddCase(actionCreator1, (state, _) => state)
           .AddCase(actionCreator2, (state, _) => state);

        Assert.Throws<ArgumentException>(AddCases);
    }

    [Fact]
    public void AddCasePayload_AddCaseTwiceForTheSameType_ThrowsArgumentException()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var actionCreator1 = new ActionCreator<string>("Type");
        var actionCreator2 = new ActionCreator<string>("Type");

        //Act
        void AddCases() => sut
           .AddCase(actionCreator1, (state, _) => state)
           .AddCase(actionCreator2, (state, _) => state);

        Assert.Throws<ArgumentException>(AddCases);
    }

    [Fact]
    public void AddCase_MatchingActionType_OnlyCaseModifiesState()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator("Type");

        //Act
        var reducer = sut
           .AddCase(actionCreator, (state, _) => state with { Value1 = "Case" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Case", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void AddCasePayload_MatchingActionType_OnlyCaseModifiesState()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator<string>("Type");

        //Act
        var reducer = sut
           .AddCase(actionCreator, (state, action) => state with { Value1 = action.Payload })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke("Payload"));

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Payload", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void AddCase_NoMatchingActionType_DefaultCaseModifiesState()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator1 = new ActionCreator("Type1");
        var actionCreator2 = new ActionCreator("Type2");

        //Act
        var reducer = sut
           .AddCase(actionCreator1, (state, _) => state with { Value1 = "Case" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator2.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Default", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void AddMatcher_MultipleMatchingMatchers_StateModifiedByAllMatchingMatchers()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator("Type");

        //Act
        var reducer = sut
           .AddMatcher(action => action.Type == "Type", (state, _) => state with { Value1 = "Matcher1" })
           .AddMatcher(action => action.Type == "Type", (state, _) => state with { Value2 = "Matcher2" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Matcher1", actual.Value1);
        Assert.Equal("Matcher2", actual.Value2);
    }

    [Fact]
    public void AddMatcher_MultipleMatchingMatchers_MatchersExecutedInOrder()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator("Type");

        //Act
        var reducer = sut
           .AddMatcher(action => action.Type == "Type", (state, _) => state with { Value1 = "Matcher1" })
           .AddMatcher(action => action.Type == "Type", (state, _) => state with { Value1 = "Matcher2" })
           .AddMatcher(action => action.Type == "Type", (state, _) => state with { Value2 = "Matcher3" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Matcher2", actual.Value1);
        Assert.Equal("Matcher3", actual.Value2);
    }

    [Fact]
    public void AddMatcher_CaseAndMultipleMatchingMatchers_CaseTriggeredBeforeMatchers()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator("Type");

        //Act
        var reducer = sut
           .AddCase(actionCreator, (state, _) => state with { Value1 = "Case" })
           .AddMatcher(
                action => action.Type == "Type",
                (state, _) => state with { Value1 = state.Value1 + "Matcher1" })
           .AddMatcher(_ => false, (state, _) => state with { Value1 = "Matcher2" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("CaseMatcher1", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void AddMatcherPayload_CaseAndMultipleMatchingMatchers_CaseTriggeredBeforeMatchers()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator<string>("Type");

        //Act
        var reducer = sut
           .AddCase(actionCreator, (state, _) => state with { Value1 = "Case" })
           .AddMatcher<string>(action => action.Type == "Type",
                (state, action) => state with { Value1 = state.Value1 + action.Payload })
           .AddMatcher(_ => false, (state, _) => state with { Value1 = "Matcher2" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke("Payload"));

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("CasePayload", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void AddDefaultCase_NoOtherCase_StateChangedByDefaultCaseReducer()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();
        var actionCreator = new ActionCreator("Type");

        //Act
        var reducer = sut
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var actual = (TestState)reducer(initialState, actionCreator.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Default", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void AddDefaultCaseTPayload_NoOtherCase_StateChangedByDefaultCaseReducerPayload()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var actionCreated = new ActionCreator<string>("Test");
        var initialState = new TestState();

        //Act
        var reducer = sut
           .AddDefaultCase<string>((state, action) => state with { Value1 = action.Payload })
           .BuildReducer(initialState);

        var action = actionCreated.Invoke(nameof(Frameworks.StateManager.Action<string>.Payload));
        var actual = (TestState)reducer(initialState, action);

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal(nameof(Frameworks.StateManager.Action<string>.Payload), actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }

    [Fact]
    public void BuildReducer_ReducerReturnNull_ThrowsNullReferenceException()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var actionCreator = new ActionCreator("Type");

        //Act
        var reducer = sut
           .AddCase(actionCreator, (_, _) => null!)
           .BuildReducer(new());

        var action = () => reducer(new TestState(), actionCreator.Invoke());

        //Assert
        Assert.Throws<NullReferenceException>(action);
    }
    
    [Fact]
    public void BuildReducer_ActionNotHandled_UsesInitialState()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var actionCreated = new ActionCreator("Test");
        var initialState = new TestState();

        //Act
        var reducer = sut.BuildReducer(initialState);
        var actual = reducer(initialState, actionCreated.Invoke());

        //Assert
        Assert.Equal(initialState, actual);
    }

    [Fact]
    public void BuildReducer_TriggerReducerTwice_BehaviorDoesntChange()
    {
        //Arrange
        var sut = new ReducerSwitchBuilder<TestState>();
        var actionCreated1 = new ActionCreator("Test1");
        var actionCreated2 = new ActionCreator<string>("Test2");
        var initialState = new TestState();

        //Act
        var reducer = sut
           .AddCase(actionCreated1, (state, _) => state with { Value1 = "Case" })
           .AddDefaultCase((state, _) => state with { Value1 = "Default" })
           .BuildReducer(initialState);

        var state = reducer(initialState, actionCreated1.Invoke());
        var actual = (TestState)reducer(state, actionCreated2.Invoke());

        //Assert
        Assert.Equal(nameof(TestState.Value1), initialState.Value1);
        Assert.Equal(nameof(TestState.Value2), initialState.Value2);

        Assert.Equal("Default", actual.Value1);
        Assert.Equal(nameof(TestState.Value2), actual.Value2);
    }
}