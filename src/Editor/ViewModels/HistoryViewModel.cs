using Editor.Models.History;
using System.Collections.ObjectModel;

namespace Editor.ViewModels;

public class HistoryViewModel : ViewModelBase
{
    private readonly History _undoRedoHistory;

    public HistoryViewModel()
    {
        _undoRedoHistory = new History();
    }

    public ObservableCollection<IHistoryCommand> UndoList => _undoRedoHistory.UndoList;
    public ObservableCollection<IHistoryCommand> RedoList => _undoRedoHistory.RedoList;

    public void AddUndo(IHistoryCommand command)
    {
        _undoRedoHistory.Add(command);
    }

    public void Undo()
    {
        _undoRedoHistory.Undo();
    }

    public void Redo()
    {
        _undoRedoHistory.Redo();
    }
}