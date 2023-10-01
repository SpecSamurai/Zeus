using DynamicData;
using DynamicData.Binding;
using Editor.Models.Logging;
using ReactiveUI;
using System.Reactive;
using System.Reactive.Linq;

namespace Editor.ViewModels.WorldEditor;

public class LogViewModel : ViewModelBase
{
    private LogLevel _filter;

    public LogViewModel()
    {
        _filter = LogLevel.Critical | LogLevel.Error | LogLevel.Warning;
        LogEntries = new();

        ToggleInformationCommand = ReactiveCommand.Create<bool, Unit>(_ =>
        {
            Filter ^= LogLevel.Information;
            return Unit.Default;
        });

        ToggleWarningCommand = ReactiveCommand.Create<bool, Unit>(_ =>
        {
            Filter ^= LogLevel.Warning;
            return Unit.Default;
        });

        ToggleErrorCommand = ReactiveCommand.Create<bool, Unit>(_ =>
        {
            Filter ^= (LogLevel.Critical | LogLevel.Error);
            return Unit.Default;
        });

        ClearCommand = ReactiveCommand.Create(() =>
        {
            Logger.Clear();
            return Unit.Default;
        });

        Func<LogEntry, bool> LogEntriesFilter(LogLevel filter) =>
            logEntry => filter.HasFlag(logEntry.LogLevel);

        var logEntriesFilter = this
            .WhenAnyValue(model => model.Filter)
            .Select(LogEntriesFilter);

        Logger.LogEntries
            .Connect()
            .RefCount()
            .Filter(logEntriesFilter)
            .Bind(LogEntries)
            .DisposeMany()
            .Subscribe();
    }

    public ReactiveCommand<bool, Unit> ToggleInformationCommand { get; }
    public ReactiveCommand<bool, Unit> ToggleWarningCommand { get; }
    public ReactiveCommand<bool, Unit> ToggleErrorCommand { get; }
    public ReactiveCommand<Unit, Unit> ClearCommand { get; }

    private LogLevel Filter
    {
        get => _filter;
        set => this.RaiseAndSetIfChanged(ref _filter, value);
    }

    public ObservableCollectionExtended<LogEntry> LogEntries { get; }
}