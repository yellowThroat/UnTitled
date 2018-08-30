#pragma once

class UndoRedoManager
{
public:
	void Clear();

	stack<class Command*> undoStack;
	stack<class Command*> redoStack;

	class Command* Undo();
	class Command* Redo();
	void AddState(class Command* state);
};
