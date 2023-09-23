using System.Collections.ObjectModel;

namespace Editor.Models.History;

public sealed class History
{
    public History()
    {
        UndoList = new ObservableCollection<IHistoryCommand>();
        RedoList = new ObservableCollection<IHistoryCommand>();
    }

    public ObservableCollection<IHistoryCommand> UndoList { get; }
    public ObservableCollection<IHistoryCommand> RedoList { get; }

    public void Add(IHistoryCommand command)
    {
        UndoList.Insert(0, command);
        RedoList.Clear();
    }

    public void Undo()
    {
        if (UndoList.Any())
        {
            var command = UndoList.First();
            UndoList.RemoveAt(0);
            command.Undo();
            RedoList.Add(command);
        }
    }

    public void Redo()
    {
        if (RedoList.Any())
        {
            var command = RedoList.Last();
            RedoList.RemoveAt(RedoList.Count - 1);
            command.Redo();
            UndoList.Insert(0, command);
        }
    }

    public void Reset()
    {
        UndoList.Clear();
        RedoList.Clear();
    }
}