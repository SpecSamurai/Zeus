using Editor.Models.History;
using System.Collections.ObjectModel;
using System.Reactive;
using ReactiveUI;

namespace Editor.ViewModels.WorldEditor;

public class HistoryViewModel : ViewModelBase
{
    public ObservableCollection<IHistoryCommand> UndoList => History.UndoList;
    public ObservableCollection<IHistoryCommand> RedoList => History.RedoList;

    public ReactiveCommand<Unit, Unit> ClearHistoryCommand { get; } = ReactiveCommand.Create(History.Reset);
}