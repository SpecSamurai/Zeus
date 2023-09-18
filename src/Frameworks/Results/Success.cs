namespace Frameworks.Results;

public record Success<TValue> : IResult<TValue>
{
    public Success(TValue value) =>
        Value = value ?? throw new ArgumentNullException(nameof(value));

    public TValue Value { get; }

    public TOutput Match<TOutput>(
        Func<TValue, TOutput> onSuccess,
        Func<ErrorMessage, TOutput> onFailure) =>
            onSuccess(Value);

    public async Task<TOutput> MatchAsync<TOutput>(
        Func<TValue, Task<TOutput>> onSuccessAsync,
        Func<ErrorMessage, TOutput> onFailure) =>
            await onSuccessAsync(Value);

    public static implicit operator TValue(Success<TValue> success) =>
        success.Value;

    public static implicit operator Success<TValue>(TValue value) =>
        new(value);
}