﻿namespace Zeus.History;

public interface IHistoryCommand
{
    string Name { get; }
    void Undo();
    void Redo();
}