using System.Net.Http.Headers;
using System.Windows.Controls;
using Zeus.Components;
using Zeus.Projects;

namespace Zeus.Views.Editors;

public partial class ProjectLayoutView : UserControl
{
    public ProjectLayoutView()
    {
        InitializeComponent();
    }

    private void OnAddGameEntityButton_Click(object sender, System.Windows.RoutedEventArgs e)
    {
        var button = sender as Button;
        var vm = button.DataContext as Scene;
        vm.AddGameEntityCommand.Execute(new GameEntity("Empty Game Entity", vm));
    }

    private void OnGameEntities_ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        var listBox = sender as ListBox;
        var newSelection = listBox.SelectedItems.Cast<GameEntity>().ToList();
        var previousSelection = newSelection.Except(e.AddedItems.Cast<GameEntity>()).Concat(e.RemovedItems.Cast<GameEntity>()).ToList();

        Project.History.Push(
            "Selection changed",
            undo: () =>
            {
                listBox.UnselectAll();
                previousSelection.ForEach(x => (listBox.ItemContainerGenerator.ContainerFromItem(x) as ListBoxItem).IsSelected = true);
            },
            redo: () =>
            {
                listBox.UnselectAll();
                newSelection.ForEach(x => (listBox.ItemContainerGenerator.ContainerFromItem(x) as ListBoxItem).IsSelected = true);
            });

        MSGameEntity msGameEntity = null;
        if (newSelection.Any())
        {
            msGameEntity = new MSGameEntity(newSelection);
        }
        GameEntityView.Instance.DataContext = msGameEntity;
    }
}
