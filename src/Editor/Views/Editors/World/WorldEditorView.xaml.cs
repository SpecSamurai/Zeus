using System.Collections.Specialized;
using System.Windows;
using System.Windows.Controls;
using Zeus.Projects;

namespace Zeus.Views.Editors;

public partial class WorldEditorView : UserControl
{
    public WorldEditorView()
    {
        InitializeComponent();
        Loaded += FocusOnLoad;
    }

    private void FocusOnLoad(object sender, RoutedEventArgs e)
    {
        Loaded -= FocusOnLoad;
        Focus();
        ((INotifyCollectionChanged)Project.History.UndoList).CollectionChanged += (s, e) => Focus();
    }
}
