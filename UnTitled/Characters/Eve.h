#pragma once
#include "Character.h"


class Eve : public Character
{
public:
	enum class PlayerAnimation
	{
		UnKnown = -1,
		Idle,
		Walk, BackWardWalk,
		Run, BackWardRun,
		TurnLeft, TurnRight,
		Jump,
		Count,
	};


public:
	Eve();
	~Eve();

	void Update() override;
	

private:
	void OpenModel();
	void InputHandle();
	void InputMove();
	void ModelRotation();
	void Play();
	void Prepare(PlayerAnimation animation);
	void DecideAction(D3DXVECTOR3& velocity);
	bool Movable();
private:
	float moveSpeed;
	float currentWaistAngle;
	float rootRotationAngle;
	float rootElapsedAngle;

	bool bRun;

	struct GamePlayerInput* input;
	struct GamePlayerSpec* spec;
	
	
private:

	PlayerAnimation prepareAnimation;
	PlayerAnimation currentAnimation;
};