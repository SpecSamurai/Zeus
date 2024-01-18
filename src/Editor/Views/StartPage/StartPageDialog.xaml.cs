using System.Windows;

namespace Editor.Views.StartPage;

public partial class StartPageDialog : Window
{
    public StartPageDialog()
    {
        InitializeComponent();
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
