namespace Frameworks.Serialization;

public interface ISerializer
{
    void ToFile<TInput>(TInput @object, string filePath);
    Task ToFileAsync<TInput>(TInput @object, string filePath, CancellationToken cancellationToken = default);

    TOutput? FromFile<TOutput>(string filePath);
    ValueTask<TOutput?> FromFileAsync<TOutput>(string filePath, CancellationToken cancellationToken = default);
}