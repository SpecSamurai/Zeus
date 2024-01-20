using System.Diagnostics;
using System.IO;
using System.Runtime.Serialization;

namespace Zeus.Serialization;

public sealed class SystemXmlSerializer : ISerializer
{
    public void ToFile<TInput>(TInput @object, string filePath)
    {
        try
        {
            using var fileStream = new FileStream(filePath, FileMode.Create);
            var serializer = new DataContractSerializer(typeof(TInput));
            serializer.WriteObject(fileStream, @object);
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
        }
    }

    public TOutput FromFile<TOutput>(string filePath)
    {
        try
        {
            using var fileStream = new FileStream(filePath, FileMode.Open);
            var serializer = new DataContractSerializer(typeof(TOutput));
            return (TOutput)serializer.ReadObject(fileStream);
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
            return default;
        }
    }
}