using Zeus.Projects;
using System.Windows;
using System.Windows.Controls;
using Zeus.StartPage;

namespace Zeus.Views.StartPage;

public partial class OpenProjectControl : UserControl
{
    public OpenProjectControl()
    {
        InitializeComponent();

        Loaded += (s, e) =>
        {
            var item = projectsListBox.ItemContainerGenerator
                .ContainerFromIndex(projectsListBox.SelectedIndex) as ListBoxItem;
            item?.Focus();
        };
    }

    private void OnOpenButton_Click(object sender, RoutedEventArgs e)
    {
        OpenSelectedProject();
    }

    private void OnListBoxItem_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
    {
        OpenSelectedProject();
    }

    private void OpenSelectedProject()
    {
        var project = OpenProjectViewModel.Open(projectsListBox.SelectedItem as ProjectData);

        var window = Window.GetWindow(this);
        window.DialogResult = project != null;
        window.DataContext = project;
        window.Close();
    }
}
