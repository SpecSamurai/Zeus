using Zeus.Projects;
using System.Windows;
using System.Windows.Controls;
using Zeus.StartPage;

namespace Zeus.Views.StartPage;

public partial class CreateProjectControl : UserControl
{
    public CreateProjectControl()
    {
        InitializeComponent();
    }

    private void OnCreateButtonClick(object sender, RoutedEventArgs e)
    {
        var vm = DataContext as NewProjectViewModel;
        var projectTempalte = templateListBox.SelectedItem as ProjectTemplate;
        var projectPath = vm.CreateProject(projectTempalte);

        var window = Window.GetWindow(this);

        var dialogResult = false;
        if (!string.IsNullOrWhiteSpace(projectPath))
        {
            dialogResult = true;
            var project = OpenProjectViewModel.Open(new()
            {
                ProjectName = vm.ProjectName,
                ProjectPath = projectPath
            });

            window.DataContext = project;
        }

        window.DialogResult = dialogResult;
        window.Close();
    }
}
