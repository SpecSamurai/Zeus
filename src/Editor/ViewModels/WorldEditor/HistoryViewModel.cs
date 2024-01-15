using Editor.Models.History;
using ReactiveUI;
using System.Collections.ObjectModel;
using System.Reactive;

namespace Editor.ViewModels.WorldEditor;

public class HistoryViewModel : ViewModelBase
{
    public ObservableCollection<IHistoryCommand> UndoList { get; }
    public ObservableCollection<IHistoryCommand> RedoList { get; }

    public ReactiveCommand<Unit, Unit> ClearHistoryCommand { get; }

    public HistoryViewModel(HistoryStack historyStack)
    {
        UndoList = historyStack.UndoList;
        RedoList = historyStack.RedoList;

        ClearHistoryCommand = ReactiveCommand.Create(historyStack.Clear);
    }
}