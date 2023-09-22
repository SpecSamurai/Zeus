using Editor.Models.Projects;

namespace Editor.Repositories;

public interface IProjectFilesRepository
{
    IReadOnlyCollection<Template> GetTemplates();
    IEnumerable<CreatedProject> GetCreatedProjects();
    void UpdateProjectsFile(string projectName, string projectPath);
}