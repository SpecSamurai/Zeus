namespace Zeus.History;

public partial class HistoryStack
{
    public void Push(string name, Action undo, Action redo)
    {
        Push(new HistoryCommand(name, undo, redo));
    }

    public void PushPropertyChange(string name, string propertyName, object instance, object undoValue, object redoValue)
    {
        Push(name,
            undo: () => instance.GetType().GetProperty(propertyName).SetValue(instance, undoValue),
            redo: () => instance.GetType().GetProperty(propertyName).SetValue(instance, redoValue));
    }
}