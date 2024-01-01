using Frameworks.StateManager;
using Frameworks.StateManager.Delegates;
using Frameworks.Tests.StateManager.Fakes;
using Xunit;

namespace Frameworks.Tests.StateManager;

public partial class StoreTest
{
    [Fact]
    public void GetState_Empty_ThrowsKeyNotFoundException()
    {
        // Arrange
        var sut = new Store();

        // Act
        var action = () => sut.GetState<string>("Slice");

        // Assert
        Assert.Throws<KeyNotFoundException>(action);
    }

    [Fact]
    public void CreateSlice_NullInitialState_ThrowsArgumentNullException()
    {
        // Arrange
        var sut = new Store();

        // Act
        var action = () => sut.CreateSlice<string?>("Slice", null, _ => { });

        // Assert
        Assert.Throws<ArgumentNullException>(action);
    }

    [Fact]
    public void CreateSlice_DuplicateSlice_ThrowsArgumentException()
    {
        // Arrange
        var sut = new Store();

        // Act
        sut.CreateSlice("Slice", new object(), _ => { });
        var action = () => sut.CreateSlice("Slice", new object(), _ => { });

        // Assert
        Assert.Throws<ArgumentException>(action);
    }

    [Fact]
    public void CreateSlice_ExtraReducers_NotIncludedInSlice()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<TestState> reducer1 = (state, _) => state;
        CaseReducer<string, TestState> reducer2 = (state, _) => state;

        var expectedActionCreators = new[]
        {
            new ActionCreator("Slice/Type1"),
            new ActionCreator<string>("Slice/Type2"),
            new ActionCreator($"Slice/{nameof(reducer1)}"),
            new ActionCreator<string>($"Slice/{nameof(reducer2)}"),
        };

        // Act
        var actual = sut.CreateSlice(
            "Slice",
            initialState,
            builder => builder
               .Add("Type1", (state, _) => state)
               .Add<string>("Type2", (state, _) => state)
               .Add(reducer1)
               .Add(reducer2),
            builder => builder.AddCase(new ActionCreator("Test"), (state, _) => state));

        // Assert
        Assert.Equal("Slice", actual.Name);
        Assert.Equal(initialState, actual.InitialState);
        Assert.Equal(expectedActionCreators.Length, actual.ActionCreators.Count);

        for (var i = 0; i < expectedActionCreators.Length; i++)
        {
            Assert.Equal(expectedActionCreators[i].GetType(), actual.ActionCreators[i].GetType());
            Assert.Equal(expectedActionCreators[i].Type, actual.ActionCreators[i].Type);
        }
    }

    [Fact]
    public void Dispatch_NonExistingSlice_ThrowsArgumentException()
    {
        // Arrange
        var sut = new Store();
        sut.CreateSlice("Slice", new object(), _ => { });

        // Act
        var action = () => sut.Dispatch("Test/Type1");

        // Assert
        Assert.Throws<ArgumentException>(action);
    }

    [Fact]
    public void Dispatch_SlideAction_StateModified()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        sut.CreateSlice(
            "Slice",
            initialState,
            builder => builder
               .Add("Type1", (state, _) => state with { Value1 = "Type1" })
               .Add<string>("Type2", (state, _) => state with { Value1 = "Type2" }));

        // Act
        sut.Dispatch("Slice/Type1");
        var actual = sut.GetState<TestState>("Slice");

        // Assert
        Assert.Equal("Type1", actual.Value1);
        Assert.Equal(initialState.Value2, actual.Value2);
    }

    [Fact]
    public void Dispatch_ActionWithoutSlicePrefix_StateChangedByExtraReducer()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        sut.CreateSlice(
            "Slice",
            initialState,
            builder => builder
               .Add("Type1", (state, _) => state with { Value1 = "Type1" })
               .Add<string>("Type2", (state, _) => state with { Value1 = "Type2" }),
            builder => builder
               .AddCase(Store.CreateAction("Type1"), (state, _) => state with { Value2 = "ExtraReducer" }));

        // Act
        sut.Dispatch("Type1");
        var actual = sut.GetState<TestState>("Slice");

        // Assert
        Assert.Equal(initialState.Value1, actual.Value1);
        Assert.Equal("ExtraReducer", actual.Value2);
    }

    [Fact]
    public void DispatchMultipleSlices_ReducerNotRelatedToSlice_ModifiesEntireStateTree()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<string, TestState> extraReducer = (state, action) => state with { Value2 = action.Payload };
        var extraAction = Store.CreateAction<string>("ExtraReducer");

        sut.CreateSlice(
            "Slice1",
            initialState,
            builder => builder
               .Add("Slice1Type1", (state, _) => state with { Value1 = "Slice1Type1" })
               .Add<string>("Slice1Type2", (state, _) => state with { Value1 = "Slice1Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        sut.CreateSlice(
            "Slice2",
            initialState,
            builder => builder
               .Add("Slice2Type1", (state, _) => state with { Value1 = "Slice2Type1" })
               .Add<string>("Slice2Type2", (state, _) => state with { Value1 = "Slice2Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        // Act
        sut.Dispatch("ExtraReducer", "Payload");
        var actual1 = sut.GetState<TestState>("Slice1");
        var actual2 = sut.GetState<TestState>("Slice2");

        // Assert
        Assert.Equal(initialState.Value1, actual1.Value1);
        Assert.Equal("Payload", actual1.Value2);

        Assert.Equal(initialState.Value1, actual2.Value1);
        Assert.Equal("Payload", actual2.Value2);
    }

    [Fact]
    public void DispatchMultipleSlices_ReducerRelatedToSlice_ModifiesOnlySlice()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<string, TestState> extraReducer = (state, action) => state with { Value2 = action.Payload };
        var extraAction = Store.CreateAction<string>("Type1");

        sut.CreateSlice(
            "Slice1",
            initialState,
            builder => builder
               .Add("Slice1/Type1", (state, _) => state with { Value1 = "Slice1Type1" })
               .Add<string>("Slice1/Type2", (state, _) => state with { Value1 = "Slice1Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        sut.CreateSlice(
            "Slice2",
            initialState,
            builder => builder
               .Add("Slice2/Type1", (state, _) => state with { Value1 = "Slice2Type1" })
               .Add<string>("Slice2/Type2", (state, _) => state with { Value1 = "Slice2Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        // Act
        sut.Dispatch("Slice1/Type1");
        var actual1 = sut.GetState<TestState>("Slice1");
        var actual2 = sut.GetState<TestState>("Slice2");

        // Assert
        Assert.Equal("Slice1Type1", actual1.Value1);
        Assert.Equal(initialState.Value2, actual1.Value2);

        Assert.Equal(initialState.Value1, actual2.Value1);
        Assert.Equal(initialState.Value2, actual2.Value2);
    }

    [Fact]
    public void Subscribe_SubscribeToDifferentSlice_NotNotified()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<string, TestState> extraReducer = (state, action) => state with { Value2 = action.Payload };
        var extraAction = Store.CreateAction<string>("Type1");

        sut.CreateSlice(
            "Slice1",
            initialState,
            builder => builder
               .Add("Slice1/Type1", (state, _) => state with { Value1 = "Slice1Type1" })
               .Add<string>("Slice1/Type2", (state, _) => state with { Value1 = "Slice1Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        sut.CreateSlice(
            "Slice2",
            initialState,
            builder => builder
               .Add("Slice2/Type1", (state, _) => state with { Value1 = "Slice2Type1" })
               .Add<string>("Slice2/Type2", (state, _) => state with { Value1 = "Slice2Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        // Act
        var actual = string.Empty;
        sut.Subscribe<TestState>("Slice2", state => actual = state.Value1);
        sut.Dispatch("Slice1/Type1");

        // Assert
        Assert.Empty(actual);
    }

    [Fact]
    public void Subscribe_SubscribeToSlice_Notified()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<string, TestState> extraReducer = (state, action) => state with { Value2 = action.Payload };
        var extraAction = Store.CreateAction<string>("Type1");

        sut.CreateSlice(
            "Slice1",
            initialState,
            builder => builder
               .Add("Slice1/Type1", (state, _) => state with { Value1 = "Slice1Type1" })
               .Add<string>("Slice1/Type2", (state, _) => state with { Value1 = "Slice1Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        sut.CreateSlice(
            "Slice2",
            initialState,
            builder => builder
               .Add("Slice2/Type1", (state, _) => state with { Value1 = "Slice2Type1" })
               .Add<string>("Slice2/Type2", (state, _) => state with { Value1 = "Slice2Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        // Act
        var actual = string.Empty;
        sut.Subscribe<TestState>("Slice1", state => actual = state.Value1);
        sut.Dispatch("Slice1/Type1");

        // Assert
        Assert.Equal("Slice1Type1", actual);
    }

    [Fact]
    public void NotifyStateChanged_SliceName_NotifiedAllSliceSubscribers()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<string, TestState> extraReducer = (state, action) => state with { Value2 = action.Payload };
        var extraAction = Store.CreateAction<string>("Type1");

        sut.CreateSlice(
            "Slice1",
            initialState,
            builder => builder
               .Add("Slice1/Type1", (state, _) => state with { Value1 = "Slice1Type1" })
               .Add<string>("Slice1/Type2", (state, _) => state with { Value1 = "Slice1Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        sut.CreateSlice(
            "Slice2",
            initialState,
            builder => builder
               .Add("Slice2/Type1", (state, _) => state with { Value1 = "Slice2Type1" })
               .Add<string>("Slice2/Type2", (state, _) => state with { Value1 = "Slice2Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        // Act
        var subscriber1 = string.Empty;
        var subscriber2 = string.Empty;
        sut.Subscribe<TestState>("Slice1", _ => subscriber1 = nameof(subscriber1));
        sut.Subscribe<TestState>("Slice2", _ => subscriber2 = nameof(subscriber2));

        sut.NotifyStateChanged("Slice1");

        // Assert
        Assert.Equal("subscriber1", subscriber1);
        Assert.Empty(subscriber2);
    }

    [Fact]
    public void Unsubscriber_Invoke_RemovesSubscriber()
    {
        // Arrange
        var sut = new Store();
        var initialState = new TestState();
        CaseReducer<string, TestState> extraReducer = (state, action) => state with { Value2 = action.Payload };
        var extraAction = Store.CreateAction<string>("Type1");

        sut.CreateSlice(
            "Slice1",
            initialState,
            builder => builder
               .Add("Slice1/Type1", (state, _) => state with { Value1 = "Slice1Type1" })
               .Add<string>("Slice1/Type2", (state, _) => state with { Value1 = "Slice1Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        sut.CreateSlice(
            "Slice2",
            initialState,
            builder => builder
               .Add("Slice2/Type1", (state, _) => state with { Value1 = "Slice2Type1" })
               .Add<string>("Slice2/Type2", (state, _) => state with { Value1 = "Slice2Type2" }),
            builder => builder.AddCase(extraAction, extraReducer));

        // Act
        var subscriber1 = string.Empty;
        var subscriber2 = string.Empty;
        var unsubscriber1 = sut.Subscribe<TestState>("Slice1", _ => subscriber1 = nameof(subscriber1));
        sut.Subscribe<TestState>("Slice2", _ => subscriber2 = nameof(subscriber2));
        unsubscriber1();

        sut.NotifyStateChanged("Slice1");

        // Assert
        Assert.Empty(subscriber1);
        Assert.Empty(subscriber2);
    }
}