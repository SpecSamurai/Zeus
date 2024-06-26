﻿using System.Diagnostics;

namespace Zeus.History;

public class HistoryCommand : IHistoryCommand
{
    private readonly Action _undo;
    private readonly Action _redo;

    public HistoryCommand(string name, Action undo, Action redo)
    {
        Debug.Assert(undo != null && redo != null);
        Name = name;
        _undo = undo;
        _redo = redo;
    }

    public string Name { get; }

    public void Undo() =>
        _undo();

    public void Redo() =>
        _redo();
}