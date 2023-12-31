using System.Text.RegularExpressions;

namespace Frameworks.StateManager;

public partial record ActionType
{
    public ActionType(string value)
    {
        Value = ValidActionType().IsMatch(value)
            ? value
            : throw new ArgumentException($"Invalid action type '{value}'.", nameof(value));

        var sliceMatch = StartsWithSlicePrefix().Match(Value);
        IsSlice = sliceMatch.Success;
        SlicePrefix = IsSlice
            ? sliceMatch.Value.TrimEnd('/')
            : string.Empty;
    }

    public string Value { get; }
    public bool IsSlice { get; }
    public string SlicePrefix { get; }

    public static implicit operator string(ActionType actionType) =>
        actionType.Value;

    public static implicit operator ActionType(string actionType) =>
        new(actionType);

    [GeneratedRegex(@"(^\w+\/\w+$)|(^\w+$)")]
    private static partial Regex ValidActionType();

    [GeneratedRegex(@"^\w+\/")] private static partial Regex StartsWithSlicePrefix();
}