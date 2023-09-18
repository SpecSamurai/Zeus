namespace Frameworks.Results;

public interface IResult<TValue>
{
    TOutput Match<TOutput>(Func<TValue, TOutput> onSuccess, Func<ErrorMessage, TOutput> onFailure);
    Task<TOutput> MatchAsync<TOutput>(
        Func<TValue, Task<TOutput>> onSuccessAsync,
        Func<ErrorMessage, TOutput> onFailure);
}