namespace Zeus.History;

public partial class HistoryStack
{
    public void Push(string name, Action undo, Action redo)
    {
        Push(new HistoryCommand(name, undo, redo));
    }
}