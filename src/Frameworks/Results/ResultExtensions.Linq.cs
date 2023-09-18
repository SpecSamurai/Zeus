namespace Frameworks.Results;

public static partial class ResultExtensions
{
    public static IResult<TOutput> SelectMany<TSource, TResult, TOutput>(
        this IResult<TSource> source,
        Func<TSource, IResult<TResult>> sourceValueSelector,
        Func<TSource, TResult, TOutput> outputSelector) =>
            source.Bind(sourceValue =>
            {
                var outputResult = sourceValueSelector(sourceValue);
                return outputResult.Map(outputValue => outputSelector(sourceValue, outputValue));
            });

    public static async Task<IResult<TOutput>> SelectMany<TSource, TResult, TOutput>(
        this Task<IResult<TSource>> source,
        Func<TSource, Task<IResult<TResult>>> sourceValueSelector,
        Func<TSource, TResult, TOutput> outputSelector)
    {
        var sourceResult = await source;
        return await sourceResult.BindAsync(async sourceValue =>
        {
            var outputResult = await sourceValueSelector(sourceValue);
            return outputResult.Map(outputValue => outputSelector(sourceValue, outputValue));
        });
    }
}