using Frameworks.StateManager;
using Xunit;

namespace Frameworks.Tests.StateManager;

public class ActionTypeTest
{
    [Theory]
    [InlineData("")]
    [InlineData("(x, y) => y")]
    [InlineData("-test")]
    [InlineData("/test")]
    [InlineData("test/")]
    [InlineData("te-st/")]
    [InlineData("te-st/test")]
    [InlineData("test/test/")]
    public void Constructor_InvalidActionTypes_ThrowsArgumentException(string actionType)
    {
        //Arrange
        //Act
        var action = () => new ActionType(actionType);

        //Assert
        Assert.Throws<ArgumentException>(action);
    }
    
    [Fact]
    public void IsSlice_NonSliceActionTypes_False()
    {
        //Arrange
        ActionType sut = "Te_st"; 
        
        //Act
        var isSlice = sut.IsSlice;
        var slicePrefix = sut.SlicePrefix;

        //Assert
        Assert.False(isSlice);
        Assert.Empty(slicePrefix);
    }
    
    [Fact]
    public void IsSlice_SliceActionTypes_True()
    {
        //Arrange
        ActionType sut = "Sli_ce/Test"; 
        
        //Act
        var isSlice = sut.IsSlice;
        var slicePrefix = sut.SlicePrefix;

        //Assert
        Assert.True(isSlice);
        Assert.Equal("Sli_ce", slicePrefix);
    }
}