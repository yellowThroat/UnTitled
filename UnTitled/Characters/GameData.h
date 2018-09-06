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
	float BattleMoveSpeed = 3.0f;

	GamePlayerSpec()
	{
		WalkSpeed = 5.0f;
		RotateSpeed = 10.0f;
		RunSpeed = 12.0f;
		BattleMoveSpeed = 3.0f;
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