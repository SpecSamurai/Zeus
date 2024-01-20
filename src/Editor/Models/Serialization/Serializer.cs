namespace Editor.Models.Serialization;

public static class Serializer
{
    private static ISerializer _serializer;

    static Serializer() =>
        _serializer = new SystemXmlSerializer();

    public static void ToFile<TInput>(this TInput @object, string filePath) =>
        _serializer.ToFile(@object, filePath);

    public static TOutput FromFile<TOutput>(string filePath) =>
        _serializer.FromFile<TOutput>(filePath);

    public static void Is(ISerializer serializer) =>
        _serializer = serializer;
}