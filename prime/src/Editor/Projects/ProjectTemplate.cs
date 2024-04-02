using System.Runtime.Serialization;

namespace Zeus.Projects;

[DataContract]
public class ProjectTemplate
{
    [DataMember] public string ProjectType { get; set; }
    [DataMember] public string ProjectFile { get; set; }
    [DataMember] public List<string> Folders { get; set; }

    public string IconFilePath { get; set; }
    public string ScreenshotFilePath { get; set; }
    public string ProjectFilePath { get; set; }

    public byte[] Icon { get; set; }
    public byte[] Screenshot { get; set; }
}
