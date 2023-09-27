using DryIoc;
using Editor.Repositories;
using Editor.Services;
using Editor.ViewModels;
using Editor.ViewModels.ProjectBrowser;
using Editor.ViewModels.WorldEditor;

namespace Editor;

internal class ServiceLocator
{
    static ServiceLocator()
    {
        Container = new Container();
        RegisterServices();
    }

    public static Container Container { get; }

    private static void RegisterServices()
    {
        Container.Register<IProjectFilesRepository, ProjectFilesRepository>();

        Container.Register<IProjectFilesService, ProjectFilesService>();

        Container.Register<MainWindowViewModel>();
        Container.Register<ProjectBrowserWindowViewModel>();
        Container.Register<WorldEditorViewModel>();
    }
}