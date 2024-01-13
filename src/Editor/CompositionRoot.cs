using Editor.Models.History;
using Editor.Models.States;
using Editor.Repositories;
using Editor.Services;
using Frameworks.StateManager;
using Splat;

namespace Editor;

internal static class CompositionRoot
{
    internal static void RegisterDependencies()
    {
        RegisterRepositories();
        RegisterServices();
        RegisterStore();
    }

    private static void RegisterRepositories()
    {
        Locator.CurrentMutable.RegisterLazySingleton<IProjectFilesRepository>(() => new ProjectFilesRepository());
    }

    private static void RegisterServices()
    {
        Locator.CurrentMutable.RegisterLazySingleton<IProjectFilesService>(() =>
        {
            var projectFilesRepository = Locator.Current.GetService<IProjectFilesRepository>();
            ArgumentNullException.ThrowIfNull(projectFilesRepository);
            return new ProjectFilesService(projectFilesRepository);
        });
    }

    private static void RegisterStore()
    {
        var store = new Store();

        store.CreateSlice<HistoryStack>(
            name: Actions.History.Slice,
            initialState: new(),
            builder => builder
                .Add<IHistoryCommand>(Actions.History.Push.Type, Reducers.History.Push)
                .Add(Actions.History.Undo.Type, Reducers.History.Undo)
                .Add(Actions.History.Redo.Type, Reducers.History.Redo)
                .Add(Actions.History.Clear.Type, Reducers.History.Clear));

        Locator.CurrentMutable.RegisterConstant(store);
    }
}
