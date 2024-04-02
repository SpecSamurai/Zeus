using Zeus.Constants;
using Zeus.Projects;
using Zeus.Serialization;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using Zeus.Logging;

namespace Zeus.StartPage;

public class OpenProjectViewModel
{
    private static readonly string _applicationDataPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Zeus");
    private static readonly string _projectDataPath;
    private static readonly ObservableCollection<ProjectData> _projects = new();

    static OpenProjectViewModel()
    {
        try
        {
            if (!Directory.Exists(_applicationDataPath)) Directory.CreateDirectory(_applicationDataPath);
            _projectDataPath = Path.Combine(_applicationDataPath, "ProjectData.xml");
            Projects = new ReadOnlyObservableCollection<ProjectData>(_projects);
            ReadProjectData();
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
            Logger.LogError("Failed to read project data");
            throw;
        }
    }

    public static ReadOnlyObservableCollection<ProjectData> Projects { get; }

    public static Project Open(ProjectData projectData)
    {
        ReadProjectData();
        var project = _projects.FirstOrDefault(x => x.FullPath == projectData.FullPath);
        if (project != null)
        {
            project.Date = DateTime.Now;
        }
        else
        {
            project = projectData;
            project.Date = DateTime.Now;
            _projects.Add(project);
        }
        WriteProjectData();

        return Project.Load(project.FullPath);
    }

    private static void ReadProjectData()
    {
        if (File.Exists(_projectDataPath))
        {
            var projects = Serializer.FromFile<ProjectDataList>(_projectDataPath).Projects.OrderByDescending(x => x.Date).ToList();
            _projects.Clear();
            foreach (var project in projects)
            {
                if (File.Exists(project.FullPath))
                {
                    project.Icon = File.ReadAllBytes(Path.Combine(project.ProjectPath, FileSystem.SystemFolderName, FileSystem.ProjectIconFileName));
                    project.Screenshot = File.ReadAllBytes(Path.Combine(project.ProjectPath, FileSystem.SystemFolderName, FileSystem.ProjectScreenshotFileName));
                    _projects.Add(project);
                }
            }
        }
    }

    private static void WriteProjectData()
    {
        var projects = _projects.OrderBy(x => x.Date).ToList();
        new ProjectDataList { Projects = projects }.ToFile(_projectDataPath);
    }
}