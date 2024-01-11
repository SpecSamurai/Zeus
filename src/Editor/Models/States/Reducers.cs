using Editor.Models.History;

namespace Editor.Models.States;

public static class Reducers
{
    public static class History
    {
        public static HistoryStack Push(HistoryStack state, Frameworks.StateManager.Action<IHistoryCommand> action)
        {
            ArgumentNullException.ThrowIfNull(action.Payload);
            state.Push(action.Payload);

            return state;
        }

        public static HistoryStack Undo(HistoryStack state, Frameworks.StateManager.Action _)
        {
            state.Undo();
            return state;
        }

        public static HistoryStack Redo(HistoryStack state, Frameworks.StateManager.Action _)
        {
            state.Redo();
            return state;
        }

        public static HistoryStack Clear(HistoryStack state, Frameworks.StateManager.Action _)
        {
            state.Clear();
            return state;
        }
    }
}