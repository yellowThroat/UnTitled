#pragma once

struct AiState;

class AiContext
{
public:
	AiContext();
	~AiContext();

	void Update();

	AiState* FindState(UINT index);

	UINT AddState(wstring name, AiState* state);
	void RemoveState(UINT index);
	void RemoveStateAll();

	void Play(UINT index);
	void Play(AiState* state);

	void Stop(UINT index);
	void Stop(AiState* state);

private:
	vector<AiState*> states;

};