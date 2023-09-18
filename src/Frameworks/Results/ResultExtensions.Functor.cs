namespace Frameworks.Results;

public static partial class ResultExtensions
{
    public static IResult<TOutput> Map<TValue, TOutput>(this IResult<TValue> result, Func<TValue, TOutput> mapping) =>
        result.Match<IResult<TOutput>>(
            onSuccess: success => Result.Success(mapping(success)),
            onFailure: Result.Failure<TOutput>);

    public static async Task<IResult<TOutput>> MapAsync<TValue, TOutput>(
        this IResult<TValue> result,
        Func<TValue, Task<TOutput>> mappingAsync) =>
            await result.MatchAsync<IResult<TOutput>>(
                onSuccessAsync: async success => Result.Success(await mappingAsync(success)),
                onFailure: Result.Failure<TOutput>);
}