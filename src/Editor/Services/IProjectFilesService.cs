using Editor.Models.Projects;
using Editor.Repositories.Models;

namespace Editor.Services;

public interface IProjectFilesService
{
    ProjectFile OpenProject(string projectFilePath);
    ProjectFile CreateProject((string RootPath, string Name) projectFolder, Template template);
}