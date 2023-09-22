using Editor.Services;
using ReactiveUI;
using System.Collections.ObjectModel;
using Editor.Repositories;
using System.Reactive;

namespace Editor.ViewModels.ProjectBrowser;

public class ProjectBrowserWindowViewModel : ViewModelBase
{
    private readonly IProjectFilesService _projectFilesService;

    private string _projectName;
    private string _projectPath;
    private bool _isValid;
    private string _errorMessage;

    private TemplateViewModel _selectedTemplate;
    private CreatedProjectViewModel? _selectedProject;

    public ProjectBrowserWindowViewModel(
        IProjectFilesRepository projectFilesRepository,
        IProjectFilesService projectFilesService)
    {
        _projectFilesService = projectFilesService;
        _projectName = "NewProject";
        _projectPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "ZeusProjects");
        _errorMessage = string.Empty;

        OpenProjectCommand = ReactiveCommand.Create(() =>
        {
            var projectFile = SelectedProject is not null
                ? _projectFilesService.OpenProject(SelectedProject.ProjectFilePath)
                : throw new InvalidOperationException("Project not selected.");

            return new ProjectViewModel(projectFile);
        });

        CreateProjectCommand = ReactiveCommand.Create(() =>
        {
            var projectFile = _projectFilesService.CreateProject((ProjectPath, ProjectName), SelectedTemplate.Template);

            return new ProjectViewModel(projectFile);
        });

        ExitProjectCommand = ReactiveCommand.Create(() => { });

        Templates = new ObservableCollection<TemplateViewModel>(projectFilesRepository
            .GetTemplates()
            .Select(TemplateViewModel.Create));
        _selectedTemplate = Templates.First();

        Projects = new ObservableCollection<CreatedProjectViewModel>(projectFilesRepository
            .GetCreatedProjects()
            .Select(CreatedProjectViewModel.Create));
        _selectedProject = Projects.FirstOrDefault();

        ValidateProjectPath();
    }

    public ReactiveCommand<Unit, ProjectViewModel> OpenProjectCommand { get; }
    public ReactiveCommand<Unit, ProjectViewModel> CreateProjectCommand { get; }
    public ReactiveCommand<Unit, Unit> ExitProjectCommand { get; }

    public ObservableCollection<TemplateViewModel> Templates { get; set; }
    public ObservableCollection<CreatedProjectViewModel> Projects { get; set; }

    public TemplateViewModel SelectedTemplate
    {
        get => _selectedTemplate;
        set => this.RaiseAndSetIfChanged(ref _selectedTemplate, value);
    }

    public CreatedProjectViewModel? SelectedProject
    {
        get => _selectedProject;
        set => this.RaiseAndSetIfChanged(ref _selectedProject, value);
    }

    public string ProjectName
    {
        get => _projectName;
        set
        {
            _projectName = value;
            ValidateProjectPath();
        }
    }

    public string ProjectPath
    {
        get => _projectPath;
        set
        {
            _projectPath = value;
            ValidateProjectPath();
        }
    }

    public bool IsValid
    {
        get => _isValid;
        set => this.RaiseAndSetIfChanged(ref _isValid, value);
    }

    public string ErrorMessage
    {
        get => _errorMessage;
        set => this.RaiseAndSetIfChanged(ref _errorMessage, value);
    }

    private void ValidateProjectPath()
    {
        var path = Path.Combine(ProjectPath, ProjectName);

        if (string.IsNullOrWhiteSpace(ProjectName.Trim()))
        {
            ErrorMessage = "Type in a project name.";
        }
        else if (ProjectName.IndexOfAny(Path.GetInvalidFileNameChars()) != -1)
        {
            ErrorMessage = "Invalid character(s) used in project name.";
        }
        else if (string.IsNullOrWhiteSpace(ProjectPath.Trim()))
        {
            ErrorMessage = "Select a valid project folder.";
        }
        else if (ProjectPath.IndexOfAny(Path.GetInvalidPathChars()) != -1)
        {
            ErrorMessage = "Invalid character(s) used in project path.";
        }
        else if (Directory.Exists(path) && Directory.EnumerateFileSystemEntries(path).Any())
        {
            ErrorMessage = "Selected project folder already exists and is not empty.";
        }
        else
        {
            ErrorMessage = string.Empty;
        }

        IsValid = ErrorMessage == string.Empty;
    }
}