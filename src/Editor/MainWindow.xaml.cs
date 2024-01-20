using Zeus.Views.StartPage;
using System.ComponentModel;
using System.Windows;
using Zeus.Projects;

namespace Zeus;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        Loaded += OnMainWindowLoaded;
        Closing += OnMainWindowClosing;
    }

    private void OnMainWindowLoaded(object sender, RoutedEventArgs e)
    {
        Loaded -= OnMainWindowLoaded;
        OpenStartPageDialog();
    }

    private void OnMainWindowClosing(object sender, CancelEventArgs e)
    {
        Closing -= OnMainWindowClosing;
        Project.Current?.Unload();
    }

    private void OpenStartPageDialog()
    {
        var dialog = new StartPageDialog() { Topmost = true };
        if (dialog.ShowDialog() == false || dialog.DataContext == null)
        {
            Application.Current.Shutdown();
        }
        else
        {
            Project.Current?.Unload();
            DataContext = dialog.DataContext;
        }
    }
}
