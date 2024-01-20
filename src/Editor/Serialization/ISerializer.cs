namespace Zeus.Serialization;

public interface ISerializer
{
    void ToFile<TInput>(TInput @object, string filePath);
    TOutput FromFile<TOutput>(string filePath);
}