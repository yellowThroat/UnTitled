#pragma once
#include "Character.h"

class Zombie : public Character
{
public:
	enum class ZombieAnimation
	{
		Unknown = -1,
		Idle, Damaged,
		Count,
	};

public:
	Zombie();
	~Zombie();
	
	void Update() override;
	void Render() override;
	bool Damaged(Character* hitter) override;

private:
	void OpenModel();
	float MoveSpeed();
	bool Movable(MoveEnd type = MoveEnd::Normal);
	void Play();
	bool Prepare(ZombieAnimation animation);
private:
	bool bRun;
	Mode mode;

	ZombieAnimation currentAnimation;
	ZombieAnimation prepareAnimation;

	struct GameZombieSpec* spec;
};