namespace Editor.Models.History;

public static partial class History
{
    public static void Add(string name, Action undo, Action redo)
    {
        Add(new HistoryCommand(name, undo, redo));
    }
}