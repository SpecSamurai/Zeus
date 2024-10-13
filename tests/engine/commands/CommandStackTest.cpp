#include <commands/CommandStack.hpp>

#include <gtest/gtest.h>

class TestCommand : public Zeus::ICommand
{
public:
    TestCommand() {};

    TestCommand(int* entity, int oldValue)
        : entity{ entity },
          oldValue{ oldValue },
          newValue{ *entity }
    {
    }

    void Execute() override
    {
        *entity = newValue;
    }

    void Undo() override
    {
        *entity = oldValue;
    }

    int* entity;
    int oldValue;
    int newValue;
};

TEST(CommandStackTest, Push_Overrides_ExistingEntries)
{
    Zeus::CommandStack<5> sut;

    EXPECT_TRUE(sut.Empty());
    EXPECT_EQ(sut.Count(), 0);

    sut.Push<TestCommand>();
    sut.Push<TestCommand>();
    sut.Push<TestCommand>();

    EXPECT_FALSE(sut.Empty());
    EXPECT_EQ(sut.Count(), 3);

    sut.Push<TestCommand>();
    sut.Push<TestCommand>();
    sut.Push<TestCommand>();

    EXPECT_FALSE(sut.Empty());
    EXPECT_EQ(sut.Count(), 5);
}

TEST(CommandStackTest, Undo)
{
    Zeus::CommandStack<5> sut;
    int testEntity = 2;

    sut.Push<TestCommand>(&testEntity, 0);
    sut.Push<TestCommand>(&testEntity, 1);

    sut.Undo();
    EXPECT_EQ(testEntity, 1);
    EXPECT_EQ(sut.Undone(), 1);

    sut.Undo();
    EXPECT_EQ(testEntity, 0);
    EXPECT_EQ(sut.Undone(), 2);

    sut.Undo();
    EXPECT_EQ(testEntity, 0);
    EXPECT_EQ(sut.Undone(), 2);
}

TEST(CommandStackTest, Redo)
{
    Zeus::CommandStack<5> sut;
    int testEntity = 0;

    testEntity = 1;
    sut.Push<TestCommand>(&testEntity, 0);

    testEntity = 2;
    sut.Push<TestCommand>(&testEntity, 1);

    sut.Undo();
    sut.Redo();
    EXPECT_EQ(testEntity, 2);
    EXPECT_EQ(sut.Undone(), 0);

    sut.Undo();
    sut.Undo();

    sut.Redo();
    EXPECT_EQ(testEntity, 1);
    EXPECT_EQ(sut.Undone(), 1);

    sut.Redo();
    EXPECT_EQ(testEntity, 2);
    EXPECT_EQ(sut.Undone(), 0);

    sut.Redo();
    EXPECT_EQ(testEntity, 2);
    EXPECT_EQ(sut.Undone(), 0);
}

TEST(CommandStackTest, PushAfterUndo_ShouldRemoveCommand)
{
    Zeus::CommandStack<5> sut;
    int testEntity = 0;

    testEntity = 1;
    sut.Push<TestCommand>(&testEntity, 0);

    testEntity = 2;
    sut.Push<TestCommand>(&testEntity, 1);

    sut.Undo();
    sut.Undo();

    testEntity = 3;
    sut.Push<TestCommand>(&testEntity, 0);

    EXPECT_EQ(sut.Undone(), 0);
    EXPECT_EQ(sut.Count(), 1);

    sut.Undo();
    EXPECT_EQ(testEntity, 0);
    EXPECT_EQ(sut.Undone(), 1);
}
