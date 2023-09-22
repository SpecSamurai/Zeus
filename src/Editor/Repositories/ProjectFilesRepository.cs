using Editor.Constants;
using Editor.Models.Projects;
using Editor.Repositories.Models;
using Frameworks.Serialization;
using System.Diagnostics;

namespace Editor.Repositories;

public class ProjectFilesRepository : IProjectFilesRepository
{
    public IReadOnlyCollection<Template> GetTemplates()
    {
        const string templatesFolderName = "ProjectTemplates";
        const string templateFileName = "template";

        try
        {
            var templatesFolderPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, templatesFolderName);
            var templateFilePaths = Directory.Exists(templatesFolderPath)
                ? Directory.GetFiles(templatesFolderPath, templateFileName, SearchOption.AllDirectories)
                : Array.Empty<string>();

            var output = new List<Template>();
            foreach (var templateFilePath in templateFilePaths)
            {
                var folderPath = Path.GetDirectoryName(templateFilePath);
                var templateFile = Serializer.FromFile<TemplateFile>(templateFilePath);

                if (folderPath is not null && templateFile is not null)
                    output.Add(new()
                    {
                        Folders = templateFile.Folders,
                        Name = templateFile.Name,
                        IconFilePath = Path.Combine(folderPath, ProjectFiles.ProjectIconFileName),
                        ScreenshotFilePath = Path.Combine(folderPath, ProjectFiles.ProjectScreenshotFileName),
                        ProjectFilePath = Path.Combine(folderPath, ProjectFiles.ProjectFileName),
                    });
            }

            return output;
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
            throw;
        }
    }

    public IEnumerable<CreatedProject> GetCreatedProjects()
    {
        try
        {
            var applicationDataFilePath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                "ZeusEditor",
                "CreatedProjects");

            var output = new List<CreatedProject>();
            if (File.Exists(applicationDataFilePath) && Serializer.FromFile<CreatedProjectsFile>(applicationDataFilePath) is CreatedProjectsFile file)
            {
                var projects = file.Projects.Select(project => new CreatedProject
                {
                    Name = project.Name,
                    ProjectFilePath = Path.Combine(project.Path, $"{project.Name}{ProjectFiles.ProjectFileExtension}"),
                    IconFilePath = Path.Combine(project.Path, ProjectFiles.SystemFolderName, ProjectFiles.ProjectIconFileName),
                    ScreenshotFilePath = Path.Combine(project.Path, ProjectFiles.SystemFolderName, ProjectFiles.ProjectScreenshotFileName)
                });

                output.AddRange(projects);
            }

            return output;
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
            throw;
        }
    }

    public void UpdateProjectsFile(string projectName, string projectPath)
    {
        try
        {
            var applicationDataFilePath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                "ZeusEditor",
                "CreatedProjects");

            var file = Serializer.FromFile<CreatedProjectsFile>(applicationDataFilePath);
            file.Projects.Add(new CreatedProjectsFileEntry
            {
                Date = DateTime.UtcNow,
                Name = projectName,
                Path = Path.Combine(projectPath, projectName)
            });

            var newConfig = new CreatedProjectsFile
            {
                Projects = file.Projects.OrderBy(data => data.Date).ToList()
            };

            newConfig.ToFile(applicationDataFilePath);
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
        }
    }
}