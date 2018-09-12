#pragma once
#include "Character.h"

class Zombie : public Character
{
public:
	enum class ZombieAiType
	{
		Unknown = -1,
		Search, Move, Attack,
		Count,
	};

	enum class ZombieState
	{
		Unknown = -1,
		Wander, Found, Return,
		Attack,
		Count,
	};

	enum class ZombieAnimation
	{
		Unknown = -1,
		Idle, Damaged, Death,
		Wander, Run,
		Attack,
		Count,
	};

public:
	Zombie();
	~Zombie();
	
	void UpdateInGame() override;
	void Update() override;
	void Render() override;
	bool Damaged(Character* hitter) override;
	void ShaderFile(Shader* val) override;

private:
	void OpenModel();
	void MakeAi();
	void ModelRotation();
	void Play();
	void DecideValid();
	bool Movable(MoveEnd type = MoveEnd::Normal);
	bool Prepare(ZombieAnimation animation);
	float MoveSpeed();

private:
	// Ai
	void OnAiMove();
	void OnAiSearch();
	void OnAiAttack();
	
private:
	float elapsedAngle;
	float rootRotationAngle;

	D3DXVECTOR3 initialPosition;

	ZombieState currentState;
	bool bRun;

	ZombieAnimation currentAnimation;
	ZombieAnimation prepareAnimation;

	struct GameZombieSpec* spec;
	class AiContext* ai;
};