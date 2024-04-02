#define TEST_UNDOREDO 1

#if TEST_UNDOREDO
#include "UndoRedoTest.hpp"
#else
#error One of the test has to be enabled
#endif

int main()
{
	UndoRedoTest undoRedoTest{};

	if (undoRedoTest.Initialize())
	{
		undoRedoTest.Run();
	}

	undoRedoTest.Shutdown();
}
