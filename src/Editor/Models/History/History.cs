using System.Collections.ObjectModel;

namespace Editor.Models.History;

public static partial class History
{
    static History()
    {
        UndoList = new ObservableCollection<IHistoryCommand>();
        RedoList = new ObservableCollection<IHistoryCommand>();
    }

    public static ObservableCollection<IHistoryCommand> UndoList { get; }
    public static ObservableCollection<IHistoryCommand> RedoList { get; }

    public static void Add(IHistoryCommand command)
    {
        UndoList.Insert(0, command);
        RedoList.Clear();
    }

    public static void Undo()
    {
        if (UndoList.Any())
        {
            var command = UndoList.First();
            UndoList.RemoveAt(0);
            command.Undo();
            RedoList.Add(command);
        }
    }

    public static void Redo()
    {
        if (RedoList.Any())
        {
            var command = RedoList.Last();
            RedoList.RemoveAt(RedoList.Count - 1);
            command.Redo();
            UndoList.Insert(0, command);
        }
    }

    public static void Reset()
    {
        UndoList.Clear();
        RedoList.Clear();
    }
}