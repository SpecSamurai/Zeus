using Frameworks.StateManager;
using Frameworks.StateManager.Builders;
using Frameworks.StateManager.Delegates;
using Frameworks.Tests.StateManager.Fakes;
using Xunit;

namespace Frameworks.Tests.StateManager.Builders;

public class SliceReducerBuilderTest
{
    [Fact]
    public void Add_InvalidActionType_ThrowsArgumentException()
    {
        //Arrange
        var sut = new SliceReducerBuilder<TestState>("Slice");

        //Act
        var action1 = () => sut.Add((state, _) => state);
        var action2 = () => sut.Add<string>((state, _) => state);

        //Assert
        Assert.Throws<ArgumentException>(action1);
        Assert.Throws<ArgumentException>(action2);
    }

    [Fact]
    public void Add_DuplicateActionType_ThrowsArgumentException()
    {
        //Arrange
        var sut = new SliceReducerBuilder<TestState>("Slice");

        //Act
        var action1 = () =>
            sut.Add("Test1", (state, _) => state)
               .Add("Test1", (state, _) => state);

        var action2 = () =>
            sut.Add<string>("Test2", (state, _) => state)
               .Add<string>("Test2", (state, _) => state);

        var action3 = () =>
            sut.Add("Slice/Type3", (state, _) => state)
               .Add("Slice/Type3", (state, _) => state);

        var action4 = () =>
            sut.Add<string>("Slice/Type4", (state, _) => state)
               .Add<string>("Slice/Type4", (state, _) => state);

        //Assert
        Assert.Throws<ArgumentException>(action1);
        Assert.Throws<ArgumentException>(action2);
        Assert.Throws<ArgumentException>(action3);
        Assert.Throws<ArgumentException>(action4);
    }

    [Fact]
    public void BuildReducers_ValidReducer_Modified()
    {
        //Arrange
        var sut = new SliceReducerBuilder<TestState>("Slice");
        var builder = new ReducerSwitchBuilder<TestState>();
        var initialState = new TestState();

        CaseReducer<TestState> reducer1 = (state, action) => state with { Value1 = action.Type };
        CaseReducer<string, TestState> reducer2 = (state, action) => state with { Value2 = action.Payload };

        var actionCreators = new[]
        {
            new ActionCreator("Slice/reducer1"),
            new ActionCreator("Slice/Type1"),
            new ActionCreator("Slice/Type2"),
            new ActionCreator<string>("Slice/reducer2"),
            new ActionCreator<string>("Slice/Type3"),
            new ActionCreator<string>("Slice/Type4")
        };

        //Act
        sut.Add(reducer1)
           .Add("Slice/Type1", (state, action) => state with { Value1 = action.Type })
           .Add("Type2", (state, action) => state with { Value1 = action.Type })
           .Add(reducer2)
           .Add<string>("Slice/Type3", (state, action) => state with { Value2 = action.Payload })
           .Add<string>("Type4", (state, action) => state with { Value2 = action.Payload })
           .BuildReducers()(builder);

        var actual = builder.BuildReducer(initialState);

        //Assert
        var state1 = (TestState)actual(initialState, actionCreators[0].Invoke());
        Assert.Equal("Slice/reducer1", state1.Value1);

        var state2 = (TestState)actual(initialState, actionCreators[1].Invoke());
        Assert.Equal("Slice/Type1", state2.Value1);

        var state3 = (TestState)actual(initialState, actionCreators[2].Invoke());
        Assert.Equal("Slice/Type2", state3.Value1);

        var actionCreator1 = ((ActionCreator<string>)actionCreators[3]).Invoke("reducer2Payload");
        var state4 = (TestState)actual(initialState, actionCreator1);
        Assert.Equal("reducer2Payload", state4.Value2);

        var actionCreator2 = ((ActionCreator<string>)actionCreators[4]).Invoke("Type3Payload");
        var state5 = (TestState)actual(initialState, actionCreator2);
        Assert.Equal("Type3Payload", state5.Value2);

        var actionCreator3 = ((ActionCreator<string>)actionCreators[5]).Invoke("Type4Payload");
        var state6 = (TestState)actual(initialState, actionCreator3);
        Assert.Equal("Type4Payload", state6.Value2);
    }

    [Fact]
    public void BuildActionCreators_ValidActionTypes_ValidActionCreators()
    {
        //Arrange
        var sut = new SliceReducerBuilder<TestState>("Slice");
        CaseReducer<TestState> reducer1 = (state, _) => state;
        CaseReducer<string, TestState> reducer2 = (state, _) => state;

        var expected = new[]
        {
            new ActionCreator($"Slice/{nameof(reducer1)}"),
            new ActionCreator<string>($"Slice/{nameof(reducer2)}"),
            new ActionCreator($"Slice/type1"),
            new ActionCreator($"Slice/type2"),
            new ActionCreator<string>($"Slice/type3"),
            new ActionCreator<string>($"Slice/type4")
        };

        //Act
        var actionCreators = sut
           .Add(reducer1)
           .Add(reducer2)
           .Add($"Slice/type1", (state, _) => state)
           .Add("type2", (state, _) => state)
           .Add<string>($"Slice/type3", (state, _) => state)
           .Add<string>("type4", (state, _) => state)
           .BuildActionCreators();

        //Assert
        Assert.Equal(expected.Length, actionCreators.Count);

        for (var i = 0; i < expected.Length; i++)
        {
            Assert.Equal(expected[i].GetType(), actionCreators[i].GetType());
            Assert.Equal(expected[i].Type, actionCreators[i].Type);
        }
    }

    [Fact]
    public void BuildActionCreators_ReuseReducers_ValidActionCreators()
    {
        //Arrange
        var sut = new SliceReducerBuilder<TestState>("Slice");
        CaseReducer<TestState> reducer1 = (state, _) => state;
        CaseReducer<string, TestState> reducer2 = (state, _) => state;

        var expected = new[]
        {
            new ActionCreator($"Slice/{nameof(reducer1)}"),
            new ActionCreator("Slice/Type1"),
            new ActionCreator("Slice/Type2"),
            new ActionCreator<string>($"Slice/{nameof(reducer2)}"),
            new ActionCreator<string>("Slice/Type3"),
            new ActionCreator<string>("Slice/Type4")
        };

        //Act
        var actionCreators = sut
           .Add(reducer1)
           .Add("Slice/Type1", reducer1)
           .Add("Slice/Type2", reducer1)
           .Add(reducer2)
           .Add("Slice/Type3", reducer2)
           .Add("Slice/Type4", reducer2)
           .BuildActionCreators();

        //Assert
        Assert.Equal(expected.Length, actionCreators.Count);

        for (var i = 0; i < expected.Length; i++)
        {
            Assert.Equal(expected[i].GetType(), actionCreators[i].GetType());
            Assert.Equal(expected[i].Type, actionCreators[i].Type);
        }
    }
}