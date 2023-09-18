namespace Frameworks.Results;

public record Failure<TValue> : IResult<TValue>
{
    public Failure(ErrorMessage errorMessage) =>
        ErrorMessage = errorMessage ?? throw new ArgumentNullException(nameof(errorMessage));

    public ErrorMessage ErrorMessage { get; }

    public TOutput Match<TOutput>(
        Func<TValue, TOutput> onSuccess,
        Func<ErrorMessage, TOutput> onFailure) =>
            onFailure(ErrorMessage);

    public Task<TOutput> MatchAsync<TOutput>(
        Func<TValue, Task<TOutput>> onSuccessAsync,
        Func<ErrorMessage, TOutput> onFailure) =>
            Task.FromResult(onFailure(ErrorMessage));

    public static implicit operator ErrorMessage(Failure<TValue> failure) =>
        failure.ErrorMessage;

    public static implicit operator Failure<TValue>(ErrorMessage errorMessage) =>
        new(errorMessage);
}