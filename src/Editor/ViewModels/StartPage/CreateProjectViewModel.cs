using Editor.Constants;
using Editor.Models;
using Editor.ViewModels.WorldEditor;
using Editor.Views.StartPage;
using ReactiveUI;
using System.Reactive;
using System.Reactive.Linq;
using System.Windows.Input;

namespace Editor.ViewModels.StartPage;

public class CreateProjectViewModel : ViewModelBase, IRoutableViewModel
{
    private const string DefaultProjectName = "NewProject";
    private bool _isValid;
    private string _errorMessage;
    private string _projectName;
    private string _projectPath;
    private ProjectTileViewModel? _selectedTemplate;

    public CreateProjectViewModel(
        IScreen screen,
        IReadOnlyList<ProjectTileViewModel> templates,
        ReactiveCommand<NewProjectData, ProjectViewModel> createProjectCommand)
    {
        _errorMessage = string.Empty;
        _projectName = DefaultProjectName;
        _projectPath = Projects.DefaultProjectsFolder;

        HostScreen = screen;
        Templates = templates;
        CreateProjectCommand = createProjectCommand;

        SelectFolderDialog = new Interaction<Unit, Uri?>();
        OpenSelectFolderDialogCommand = ReactiveCommand.CreateFromTask(async () =>
        {
            var uri = await SelectFolderDialog.Handle(Unit.Default);
            if (uri is not null) ProjectPath = uri.AbsolutePath;
        });

        ValidateProjectPath();
    }

    public IScreen HostScreen { get; }
    public string? UrlPathSegment { get; } = nameof(CreateProjectView);
    public IReadOnlyList<ProjectTileViewModel> Templates { get; }
    public ReactiveCommand<NewProjectData, ProjectViewModel> CreateProjectCommand { get; }
    public Interaction<Unit, Uri?> SelectFolderDialog { get; }
    public ICommand OpenSelectFolderDialogCommand { get; }

    public NewProjectData NewProjectData =>
        new()
        {
            Name = _projectName,
            Path = _projectPath,
            Template = _selectedTemplate
        };

    public ProjectTileViewModel? SelectedTemplate
    {
        get => _selectedTemplate;
        set => _selectedTemplate = value;
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
            this.RaiseAndSetIfChanged(ref _projectPath, value);
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