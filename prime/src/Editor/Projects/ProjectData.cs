using System.IO;
using System.Runtime.Serialization;

namespace Zeus.Projects;

[DataContract]
public class ProjectData
{
    [DataMember] public string ProjectName { get; set; }
    [DataMember] public string ProjectPath { get; set; }
    [DataMember] public DateTime Date { get; set; }

    public string FullPath => Path.Combine(ProjectPath, $"{ProjectName}{Project.Extension}");

    public byte[] Icon { get; set; }
    public byte[] Screenshot { get; set; }
}
