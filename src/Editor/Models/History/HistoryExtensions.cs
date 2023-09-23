namespace Editor.Models.History;

public static class HistoryExtensions
{
    public static void Add(this History history, string name, Action undo, Action redo)
    {
        history.Add(new HistoryCommand(name, undo, redo));
    }
}