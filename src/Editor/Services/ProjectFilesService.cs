using Editor.Constants;
using Editor.Models.Projects;
using Editor.Repositories;
using Editor.Repositories.Models;
using Frameworks.Serialization;
using System.Runtime.Serialization;
using Editor.Models.Logging;

namespace Editor.Services;

public class ProjectFilesService : IProjectFilesService
{
    private readonly IProjectFilesRepository _projectFilesRepository;

    public ProjectFilesService(IProjectFilesRepository projectFilesRepository)
    {
        _projectFilesRepository = projectFilesRepository;
    }

    public ProjectFile OpenProject(string projectFilePath)
    {
        try
        {
            var projectFile = File.Exists(projectFilePath)
                ? Serializer.FromFile<ProjectFile>(projectFilePath)
                : default;

            return projectFile ?? throw new SerializationException("Could not deserialize project file.");
        }
        catch (Exception e)
        {
            Logger.LogError(e.Message);
            throw;
        }
    }

    public ProjectFile CreateProject((string RootPath, string Name) projectFolder, Template template)
    {
        try
        {
            var gameProjectPath = Path.Combine(projectFolder.RootPath, projectFolder.Name);
            if (!Directory.Exists(gameProjectPath)) Directory.CreateDirectory(gameProjectPath);

            var systemFolderPath = Path.Combine(gameProjectPath, ProjectFiles.SystemFolderName);

            var dirInfo = Directory.CreateDirectory(systemFolderPath);
            dirInfo.Attributes |= FileAttributes.Hidden;

            File.Copy(template.IconFilePath, Path.Combine(systemFolderPath, ProjectFiles.ProjectIconFileName));
            File.Copy(template.ScreenshotFilePath, Path.Combine(systemFolderPath, ProjectFiles.ProjectScreenshotFileName));

            foreach (var folder in template.Folders)
            {
                Directory.CreateDirectory(Path.Combine(gameProjectPath, folder));
            }

            var projectJson = File.ReadAllText(template.ProjectFilePath);
            projectJson = projectJson.Replace("<name>", projectFolder.Name).Replace("<path>", gameProjectPath.Replace(@"\", @"\\"));
            var projectPath = Path.GetFullPath(Path.Combine(gameProjectPath, $"{projectFolder.Name}{ProjectFiles.ProjectFileExtension}"));
            File.WriteAllText(projectPath, projectJson);

            _projectFilesRepository.UpdateProjectsFile(projectFolder.Name, gameProjectPath);

            return Serializer.FromFile<ProjectFile>(projectPath)
                ?? throw new SerializationException($"Could not deserialize file {projectPath}.");
        }
        catch (Exception e)
        {
            Logger.LogError(e.Message);
            throw;
        }
    }
}