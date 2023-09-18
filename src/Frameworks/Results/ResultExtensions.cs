namespace Frameworks.Results;

public static partial class ResultExtensions
{
    public static Success<TValue> ToSuccess<TValue>(this TValue success) =>
        success;

    public static bool IsSuccess<TValue>(this IResult<TValue> result) =>
        result.Match(onSuccess: _ => true, onFailure: _ => false);

    public static bool IsFailure<TValue>(this IResult<TValue> result) =>
        result.Match(onSuccess: _ => false, onFailure: _ => true);

    public static TValue GetValueOrFallback<TValue>(this IResult<TValue> result, TValue fallback) =>
        result.Match(onSuccess: value => value, onFailure: _ => fallback);
}