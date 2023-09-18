namespace Frameworks.Results;

// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/unit-type
public sealed record Unit
{
    public static Unit New() =>
        new();

    public override string ToString() =>
        "()";
}