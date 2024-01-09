using Editor.Models.History;
using Frameworks.StateManager;

namespace Editor.Models.States;

public static class Actions
{
    public static class History
    {
        public const string Slice = nameof(History);

        public static readonly ActionCreator<IHistoryCommand> Add = Store.CreateAction<IHistoryCommand>($"{Slice}/{nameof(Add)}");
        public static readonly ActionCreator Undo = Store.CreateAction($"{Slice}/{nameof(Undo)}");
        public static readonly ActionCreator Redo = Store.CreateAction($"{Slice}/{nameof(Redo)}");
        public static readonly ActionCreator Reset = Store.CreateAction($"{Slice}/{nameof(Reset)}");
    }
}