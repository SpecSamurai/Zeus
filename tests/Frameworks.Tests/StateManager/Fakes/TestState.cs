namespace Frameworks.Tests.StateManager.Fakes;

public record TestState
{
    public string? Value1 { get; init; } = nameof(Value1);
    public string? Value2 { get; init; } = nameof(Value2);
}