using Zeus.Constants;
using Zeus.Projects;
using Zeus.Serialization;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using Zeus.Common;
using Zeus.Logging;

namespace Zeus.StartPage;

public class NewProjectViewModel : ViewModelBase
{
    private bool _isValid;
    private string _errorMessage = string.Empty;
    private readonly string _templatePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ProjectTemplates");
    private string _projectName = "NewProject";
    private string _projectPath = FileSystem.DefaultProjectsFolder;
    private readonly ObservableCollection<ProjectTemplate> _projectTemplates = new ObservableCollection<ProjectTemplate>();

    public NewProjectViewModel()
    {
        ProjectTemplates = new(_projectTemplates);
        try
        {
            var files = Directory.GetFiles(_templatePath, "template.xml", SearchOption.AllDirectories);
            Debug.Assert(files.Length > 0);
            foreach (var file in files)
            {
                var template = Serializer.FromFile<ProjectTemplate>(file);
                ArgumentNullException.ThrowIfNull(template);
                var dirName = Path.GetDirectoryName(file);
                ArgumentNullException.ThrowIfNull(dirName);

                template.IconFilePath = Path.Combine(dirName, FileSystem.ProjectIconFileName);
                template.Icon = File.ReadAllBytes(template.IconFilePath);
                template.ScreenshotFilePath = Path.Combine(dirName, FileSystem.ProjectScreenshotFileName);
                template.Screenshot = File.ReadAllBytes(template.ScreenshotFilePath);
                template.ProjectFilePath = Path.Combine(dirName, template.ProjectFile);

                _projectTemplates.Add(template);
            }

            ValidateProjectPath();
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
            Logger.LogError("Failed to read project templates");
            throw;
        }
    }

    public string CreateProject(ProjectTemplate template)
    {
        ValidateProjectPath();
        if (!IsValid) return string.Empty;

        var path = Path.Combine(ProjectPath, ProjectName);

        try
        {
            if (!Directory.Exists(path)) Directory.CreateDirectory(path);
            foreach (var folder in template.Folders)
            {
                Directory.CreateDirectory(Path.Combine(path, folder));
            }

            var dirInfo = new DirectoryInfo(Path.Combine(path, FileSystem.SystemFolderName));
            dirInfo.Attributes |= FileAttributes.Hidden;

            File.Copy(template.IconFilePath, Path.Combine(dirInfo.FullName, FileSystem.ProjectIconFileName));
            File.Copy(template.ScreenshotFilePath, Path.Combine(dirInfo.FullName, FileSystem.ProjectScreenshotFileName));

            var projectFile = File.ReadAllText(template.ProjectFilePath);
            projectFile = string.Format(projectFile, ProjectName, ProjectPath);
            var projectPath = Path.Combine(path, $"{ProjectName}{FileSystem.ProjectFileExtension}");
            File.WriteAllText(projectPath, projectFile);

            return path;
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
            Logger.LogError($"Failed to create {ProjectName}");
            throw;
        }
    }

    public ReadOnlyObservableCollection<ProjectTemplate> ProjectTemplates { get; }

    public string ProjectName
    {
        get => _projectName;
        set
        {
            if (_projectName != value)
            {
                _projectName = value;
                ValidateProjectPath();
                OnPropertyChanged(nameof(ProjectName));
            }
        }
    }

    public string ProjectPath
    {
        get => _projectPath;
        set
        {
            if (_projectPath != value)
            {
                _projectPath = value;
                ValidateProjectPath();
                OnPropertyChanged(nameof(ProjectPath));
            }
        }
    }

    public bool IsValid
    {
        get => _isValid;
        set
        {
            if (_isValid != value)
            {
                _isValid = value;
                OnPropertyChanged(nameof(IsValid));
            }
        }
    }

    public string ErrorMessage
    {
        get => _errorMessage;
        set
        {
            if (_errorMessage != value)
            {
                _errorMessage = value;
                OnPropertyChanged(nameof(ErrorMessage));
            }
        }
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
