namespace Frameworks.Results;

public static partial class ResultExtensions
{
    public static IResult<TResult> Bind<TValue, TResult>(
        this IResult<TValue> result,
        Func<TValue, IResult<TResult>> mapping) =>
            result.Map(mapping).Flatten();

    public static async Task<IResult<TResult>> BindAsync<TValue, TResult>(
        this IResult<TValue> result,
        Func<TValue, Task<IResult<TResult>>> mapperAsync) =>
            (await result.MapAsync(mapperAsync)).Flatten();

    public static IResult<TValue> Flatten<TValue>(this IResult<IResult<TValue>> result) =>
        result.Match(
            onSuccess: success => success,
            onFailure: Result.Failure<TValue>);
}