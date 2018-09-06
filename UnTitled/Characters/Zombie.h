#pragma once
#include "Character.h"

class Zombie : public Character
{
public:
	enum class ZombieAnimation
	{
		Unknown = -1,
		Idle, Walk, Run,
		Attack,
		Count,
	};

public:
	Zombie();
	~Zombie();
	
	void Update() override;
	void Render() override;

private:
	void OpenModel();
	float MoveSpeed();
	bool Movable(MoveEnd type = MoveEnd::Normal);

private:
	bool bRun;
	Mode mode;

	ZombieAnimation currentAnimation;
	ZombieAnimation prepareAnimation;

	struct GameZombieSpec* spec;
};