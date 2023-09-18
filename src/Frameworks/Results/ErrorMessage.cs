namespace Frameworks.Results;

public record ErrorMessage
{
    public ErrorMessage(string code, params (string Key, object Value)[] parameters)
    {
        Code = code;
        Data = parameters.ToDictionary(parameter => parameter.Key, parameter => parameter.Value);
    }

    public void Deconstruct(out string code, out IReadOnlyDictionary<string, object> data)
    {
        code = Code;
        data = Data;
    }

    public string Code { get; }
    public IReadOnlyDictionary<string, object> Data { get; }

    public static implicit operator string(ErrorMessage errorMessage) =>
        errorMessage.Code;
}