#pragma once

namespace Zeus
{
class IUndoableCommand
{
public:
    virtual ~IUndoableCommand()
    {
    }

    virtual void Execute() = 0;
    virtual void Undo() = 0;
};
}
