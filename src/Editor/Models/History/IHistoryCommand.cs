namespace Zeus.Models.History;

public interface IHistoryCommand
{
    string Name { get; }
    void Undo();
    void Redo();
}