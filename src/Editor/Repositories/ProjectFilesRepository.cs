using Editor.Constants;
using Editor.Models.Projects;
using Editor.Repositories.Models;
using Frameworks.Serialization;
using Editor.Models.Logging;
using Editor.Services;

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
            var projectTemplates = FileManager.GetFolder(templatesFolderPath);

            var output = new List<Template>();
            foreach (var folder in projectTemplates.Folders)
            {
                var templateFile = folder.GetFileContent<TemplateFile>(templateFileName);

                if (templateFile is not null)
                    output.Add(new()
                    {
                        Folders = templateFile.Folders,
                        Name = templateFile.Name,
                        IconFilePath = Path.Combine(folder.FullPath, Projects.ProjectIconFileName),
                        ScreenshotFilePath = Path.Combine(folder.FullPath, Projects.ProjectScreenshotFileName),
                        ProjectFilePath = Path.Combine(folder.FullPath, Projects.ProjectFileName),
                    });
            }

            return output;
        }
        catch (Exception e)
        {
            Logger.LogError(e.Message);
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
            if (File.Exists(applicationDataFilePath) && Serializer.FromFile<CreatedProjectsFile>(applicationDataFilePath) is { } file)
            {
                var projects = file.Projects.Select(project => new CreatedProject
                {
                    Name = project.Name,
                    ProjectFilePath = Path.Combine(project.Path, $"{project.Name}{Projects.ProjectFileExtension}"),
                    IconFilePath = Path.Combine(project.Path, Projects.SystemFolderName, Projects.ProjectIconFileName),
                    ScreenshotFilePath = Path.Combine(project.Path, Projects.SystemFolderName, Projects.ProjectScreenshotFileName)
                });

                output.AddRange(projects);
            }

            return output;
        }
        catch (Exception e)
        {
            Logger.LogError(e.Message);
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
            Logger.LogError(e.Message);
            throw;
        }
    }
}