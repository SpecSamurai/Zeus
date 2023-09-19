namespace Frameworks.Serialization;

public static class Serializer
{
    private static ISerializer _serializer;

    static Serializer() =>
        _serializer = new SystemJsonSerializer();

    public static void ToFile<TInput>(this TInput @object, string filePath) =>
        _serializer.ToFile(@object, filePath);

    public static Task ToFileAsync<TInput>(this TInput @object, string filePath, CancellationToken cancellationToken = default) =>
        _serializer.ToFileAsync(@object, filePath, cancellationToken);

    public static TOutput? FromFile<TOutput>(string filePath) =>
        _serializer.FromFile<TOutput>(filePath);

    public static ValueTask<TOutput?> FromFileAsync<TOutput>(string filePath, CancellationToken cancellationToken = default) =>
        _serializer.FromFileAsync<TOutput>(filePath, cancellationToken);

    public static void Is(ISerializer serializer) =>
        _serializer = serializer;
}