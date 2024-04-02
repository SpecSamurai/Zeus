using System.Runtime.Serialization;

namespace Zeus.Projects;

[DataContract]
public class ProjectDataList
{
    [DataMember]
    public List<ProjectData> Projects { get; set; }
}
