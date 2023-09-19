using System.Text.Json;
using System.Text.Json.Serialization;

namespace Frameworks.Serialization;

public sealed class SystemJsonSerializer : ISerializer
{
    private readonly JsonSerializerOptions _options = new()
    {
        WriteIndented = true,
        ReferenceHandler = ReferenceHandler.Preserve
    };

    public void ToFile<TInput>(TInput @object, string filePath)
    {
        using var fileStream = File.OpenWrite(filePath);
        JsonSerializer.Serialize(fileStream, @object, _options);
    }

    public Task ToFileAsync<TInput>(TInput @object, string filePath, CancellationToken cancellationToken = default)
    {
        using var fileStream = File.OpenWrite(filePath);
        return JsonSerializer.SerializeAsync(fileStream, @object, _options, cancellationToken);
    }

    public TOutput? FromFile<TOutput>(string filePath)
    {
        using var fileStream = File.OpenRead(filePath);
        return JsonSerializer.Deserialize<TOutput>(fileStream, _options);
    }

    public ValueTask<TOutput?> FromFileAsync<TOutput>(string filePath, CancellationToken cancellationToken = default)
    {
        using var fileStream = File.OpenRead(filePath);
        return JsonSerializer.DeserializeAsync<TOutput>(fileStream, _options, cancellationToken);
    }
}