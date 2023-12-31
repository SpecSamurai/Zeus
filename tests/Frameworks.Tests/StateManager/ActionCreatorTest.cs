using Frameworks.StateManager;
using Xunit;
using Action = Frameworks.StateManager.Action;

namespace Frameworks.Tests.StateManager;

public class ActionCreatorTest
{
    [Fact]
    public void Match_DifferentActionType_False()
    {
        //Arrange
        var sut = new ActionCreator("Type1");

        //Act
        var actual = sut.Match(new Action("Type2"));

        //Assert
        Assert.False(actual);
    }

    [Fact]
    public void Match_SameActionType_True()
    {
        //Arrange
        var sut = new ActionCreator("Type");

        //Act
        var actual = sut.Match(new Action("Type"));

        //Assert
        Assert.True(actual);
    }

    [Fact]
    public void Invoke_CreatedType()
    {
        //Arrange
        var sut = new ActionCreator("Type");

        //Act
        var actual = sut.Invoke().Type;

        //Assert
        Assert.Equal("Type", actual);
    }
}