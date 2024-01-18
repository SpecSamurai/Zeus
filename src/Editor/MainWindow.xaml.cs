using Editor.Views.StartPage;
using System.Windows;

namespace Editor;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        Loaded += OnMainWindowLoaded;
    }

    private void OnMainWindowLoaded(object sender, RoutedEventArgs e)
    {
        Loaded -= OnMainWindowLoaded;
        OpenStartPageDialog();
    }

    private void OpenStartPageDialog()
    {
        var dialog = new StartPageDialog();
        if (dialog.ShowDialog() == false)
        {
            Application.Current.Shutdown();
        }
        else
        {

        }
    }
}