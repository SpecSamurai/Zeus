using System.Windows.Controls;
using Zeus.Components;
using Zeus.Projects;

namespace Zeus.Views.Editors;

public partial class GameEntityView : UserControl
{
    public static Action _undoAction { get; private set; }
    public static string _propertyName { get; private set; }
    public static GameEntityView Instance { get; private set; }

    public GameEntityView()
    {
        InitializeComponent();
        DataContext = null;
        Instance = this;
        DataContextChanged += (_, _) =>
        {
            if (DataContext != null)
            {
                (DataContext as MSEntity).PropertyChanged += (s, e) => _propertyName = e.PropertyName;
            }
        };
    }

    private Action GetRenameAction()
    {
        var vm = DataContext as MSEntity;
        var selection = vm.SelectedEntities.Select(entity => (entity, entity.Name)).ToList();
        return new Action(() =>
        {
            selection.ForEach(x => x.entity.Name = x.Name);
            (DataContext as MSEntity).Refresh();
        });
    }

    private Action GetIsEnabledAction()
    {
        var vm = DataContext as MSEntity;
        var selection = vm.SelectedEntities.Select(entity => (entity, entity.IsEnabled)).ToList();
        return new Action(() =>
        {
            selection.ForEach(x => x.entity.IsEnabled = x.IsEnabled);
            (DataContext as MSEntity).Refresh();
        });
    }

    private void OnName_TextBox_GotKeyboardFocus(object sender, System.Windows.Input.KeyboardFocusChangedEventArgs e)
    {
        _undoAction = GetRenameAction();
    }

    private void OnName_TextBox_LostKeyboardFocus(object sender, System.Windows.Input.KeyboardFocusChangedEventArgs e)
    {
        if (_propertyName == nameof(MSEntity.Name) && _undoAction != null)
        {
            var redoAction = GetRenameAction();
            Project.History.Push("Rename game entity", _undoAction, redoAction);
            _propertyName = null;
        }
        _undoAction = null;
    }

    private void OnEnabled_ToggleButton_Click(object sender, System.Windows.RoutedEventArgs e)
    {
        var undoAction = GetIsEnabledAction();
        var vm = DataContext as MSEntity;
        vm.IsEnabled = (sender as CheckBox).IsChecked == true;
        var redoAction = GetIsEnabledAction();
        Project.History.Push(vm.IsEnabled == true ? "Enabled game entity" : "Disabled game entity", undoAction, redoAction);
    }
}
