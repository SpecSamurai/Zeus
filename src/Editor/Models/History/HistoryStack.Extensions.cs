namespace Editor.Models.History;

public partial class HistoryStack
{
    public void Add(string name, Action undo, Action redo)
    {
        Push(new HistoryCommand(name, undo, redo));
    }
}