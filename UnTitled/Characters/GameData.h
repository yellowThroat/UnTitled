#pragma once
#include "stdafx.h"

struct GameCharacterSpec
{
	virtual ~GameCharacterSpec() {}

	float WalkSpeed;
	float RotateSpeed;
	float RunSpeed;
	float HealthPoint;
};

struct GamePlayerSpec : public GameCharacterSpec
{
	float BoxingStepSpeed;
	float BattleWalkSpeed;
	float BattleRunSpeed;
	float TargetAngle;
	float TargetDistance;

	GamePlayerSpec()
	{
		HealthPoint = 100.0f;
		WalkSpeed = 5.0f;
		RotateSpeed = 10.0f;
		RunSpeed = 12.0f;
		BoxingStepSpeed = 3.0f;
		BattleWalkSpeed = 5.0f;
		BattleRunSpeed = 10.0f;
		TargetAngle = 45.0f;
		TargetDistance = 15.0f;
	}
	~GamePlayerSpec() {}
};

struct MonsterSpec : GameCharacterSpec
{
	float SearchRange;
	float MoveRange;
	float attackRange;

	MonsterSpec()
	{
		SearchRange = 20.0f;
		MoveRange = 50.0f;
		attackRange = 0.5f;
	}

	~MonsterSpec() {}
};

struct GameZombieSpec : public MonsterSpec
{
	GameZombieSpec()
	{
		HealthPoint = 30.0f;
		WalkSpeed = 1.0f;
		RunSpeed = 10.0f;
		RotateSpeed = 5.0f;
		attackRange = 3.0f;
	}
};