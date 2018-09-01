#pragma once
#include "Character.h"

class Eve : public Character
{
public:
	Eve();
	~Eve();

	void Update() override;
	

private:
	void OpenModel();
	void InputHandle();
	void InputMove();

private:
	float moveSpeed;
	bool bRun;
	struct GamePlayerInput* input;
	struct GamePlayerSpec* spec;

};