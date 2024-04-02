using System.Windows.Controls;
using Zeus.Logging;

namespace Zeus.Views;

public partial class LogView : UserControl
{
    public LogView()
    {
        InitializeComponent();
    }

    private void OnClear_Button_Click(object sender, System.Windows.RoutedEventArgs e)
    {
        Logger.Clear();
    }

    private void OnLogFilter_Button_Click(object sender, System.Windows.RoutedEventArgs e)
    {
        LogLevel filter = 0;

        if (toggleInfo.IsChecked == true) filter |= LogLevel.Info;
        if (toggleWarn.IsChecked == true) filter |= LogLevel.Warning;
        if (toggleError.IsChecked == true) filter |= LogLevel.Error;

        Logger.SetFilter(filter);
    }
}
