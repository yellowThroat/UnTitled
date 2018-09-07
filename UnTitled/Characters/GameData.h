#pragma once
#include "stdafx.h"

struct GameCharacterSpec
{
	float WalkSpeed;
	float RotateSpeed;
	float RunSpeed;
};

struct GamePlayerSpec : public GameCharacterSpec
{
	float BoxingStepSpeed = 3.0f;
	float BattleWalkSpeed;
	float BattleRunSpeed;
	GamePlayerSpec()
	{
		WalkSpeed = 5.0f;
		RotateSpeed = 10.0f;
		RunSpeed = 12.0f;
		BoxingStepSpeed = 3.0f;
		BattleWalkSpeed = 5.0f;
		BattleRunSpeed = 10.0f;
	}
};

struct GameZombieSpec : public GameCharacterSpec
{
	GameZombieSpec()
	{
		WalkSpeed = 3.0f;
		WalkSpeed = 10.0f;
		RotateSpeed = 7.0f;
	}
};