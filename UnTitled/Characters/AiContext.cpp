#include "stdafx.h"
#include "AiContext.h"
#include "AiState.h"


AiContext::AiContext()
{
}

AiContext::~AiContext()
{
	for (auto state : states)
		SAFE_DELETE(state);
}

void AiContext::Update()
{
	for (auto state : states)
		state->Update();
}

AiState * AiContext::FindState(UINT index)
{
	return states[index];
}

UINT AiContext::AddState(wstring name, AiState * state)
{
	state->Name = name;
	states.push_back(state);
	return states.size() - 1;
}

void AiContext::RemoveState(UINT index)
{
	states.erase(states.begin() + index);
}

void AiContext::RemoveStateAll()
{
	states.clear();
}

void AiContext::Play(UINT index)
{
	AiState* state = FindState(index);
	Play(state);
}

void AiContext::Play(AiState * state)
{
	state->bActive = true;

	state->Start();
}

void AiContext::Stop(UINT index)
{
	Stop(FindState(index));
}

void AiContext::Stop(AiState * state)
{
	state->Reset();
}
