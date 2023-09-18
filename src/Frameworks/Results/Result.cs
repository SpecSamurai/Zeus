namespace Frameworks.Results;

public static class Result
{
    public static Success<TValue> Success<TValue>(TValue success) =>
        success;

    public static Failure<TValue> Failure<TValue>(ErrorMessage failure) =>
        failure;

    public static Failure<TValue> Failure<TValue>(string code, params (string Key, object Value)[] parameters) =>
        new(new ErrorMessage(code, parameters));
}