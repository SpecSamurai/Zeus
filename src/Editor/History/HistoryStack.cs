using System.Collections.ObjectModel;

namespace Zeus.History;

public partial class HistoryStack
{
    private bool _enableAdd = true;
    private readonly ObservableCollection<IHistoryCommand> _undoList = new ObservableCollection<IHistoryCommand>();
    private readonly ObservableCollection<IHistoryCommand> _redoList = new ObservableCollection<IHistoryCommand>();

    public HistoryStack()
    {
        UndoList = new ReadOnlyObservableCollection<IHistoryCommand>(_undoList);
        RedoList = new ReadOnlyObservableCollection<IHistoryCommand>(_redoList);
    }

    public ReadOnlyObservableCollection<IHistoryCommand> UndoList { get; }
    public ReadOnlyObservableCollection<IHistoryCommand> RedoList { get; }

    public void Push(IHistoryCommand command)
    {
        if (_enableAdd)
        {
            _undoList.Add(command);
            _redoList.Clear();
        }
    }

    public void Undo()
    {
        if (_undoList.Any())
        {
            var command = _undoList.Last();
            _undoList.RemoveAt(_undoList.Count - 1);
            _enableAdd = false;
            command.Undo();
            _redoList.Insert(0, command);
            _enableAdd = true;
        }
    }

    public void Redo()
    {
        if (_redoList.Any())
        {
            var command = _redoList.First();
            _redoList.RemoveAt(0);
            _enableAdd = false;
            command.Redo();
            _enableAdd = true;
            _undoList.Add(command);
        }
    }

    public void Clear()
    {
        _undoList.Clear();
        _redoList.Clear();
    }
}
