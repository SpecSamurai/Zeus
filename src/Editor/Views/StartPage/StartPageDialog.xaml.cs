using System.Windows;
using Zeus.StartPage;

namespace Zeus.Views.StartPage;

public partial class StartPageDialog : Window
{
    public StartPageDialog()
    {
        InitializeComponent();
        Loaded += OnLoaded;
    }

    private void OnLoaded(object sender, RoutedEventArgs e)
    {
        Loaded -= OnLoaded;
        if(OpenProjectViewModel.Projects.Count == 0)
        {
            openProjectButton.IsEnabled = false;
            openProjectView.Visibility = Visibility.Hidden;
            OnToggleButton_Click(createProjectButton, new());
        }
    }

    private void OnToggleButton_Click(object sender, RoutedEventArgs e)
    {
        if(sender == openProjectButton)
        {
            if(createProjectButton.IsChecked == true)
            {
                createProjectButton.IsChecked = false;
                startPageContent.Margin = new Thickness(0);
            }
            
            openProjectButton.IsChecked = true;
        }
        else
        {
            if(openProjectButton.IsChecked == true)
            {
                openProjectButton.IsChecked = false;
                startPageContent.Margin = new Thickness(-800, 0, 0, 0);
            }
            
            createProjectButton.IsChecked = true;
        }
    }
}
