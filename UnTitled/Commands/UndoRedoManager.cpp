#include "stdafx.h"
#include "UndoRedoManager.h"
#include "Command.h"

void UndoRedoManager::Clear()
{
	for (UINT i = 0; i < undoStack.size();)
	{
		Command* state = undoStack.top();
		undoStack.pop();
		SAFE_DELETE(state);
	}

	for (UINT i = 0; i < redoStack.size();)
	{
		Command* state = redoStack.top();
		redoStack.pop();
		SAFE_DELETE(state);
	}
}

Command * UndoRedoManager::Undo()
{
	if (undoStack.empty()) return NULL;
	Command* state = undoStack.top();
	state->Undo();
	undoStack.pop();
	redoStack.push(state);
	return state;
}

Command * UndoRedoManager::Redo()
{
	if (redoStack.empty())return NULL;
	Command* state = redoStack.top();
	state->Redo();
	redoStack.pop();
	undoStack.push(state);
	return state;
}

void UndoRedoManager::AddState(Command * state)
{
	undoStack.push(state);
	for (UINT i = 0; i < redoStack.size();)
	{
		Command* state = redoStack.top();
		redoStack.pop();
		SAFE_DELETE(state);
	}

}
