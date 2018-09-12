#pragma once
#include "Character.h"

namespace Sahpes
{
	class Sphere;
}

class Eve : public Character
{
public:
	enum class PlayerAnimation
	{
		UnKnown = -1,
		Idle, Walk, Run,
		Jump,
		Boxing_Idle, Boxing_Step,
		OnePunch, TwoPunch, Kick,
		OneHand_Idle, OneHand_Walk, OneHand_Run,
		OneHand_First, OneHand_Second, OneHand_Last,
		Damaged,
		Count,
	};

	enum class WeaponType
	{
		UnArmed = -1,
		Fist,
		OneHand,
		Count,
	};

public:
	Eve(ExecuteValues* values);
	~Eve();
	void UpdateInGame() override;
	void Update() override;
	void Render() override;
	bool Damaged(Character* hitter) override;
	void SearchTarget(Character* character);
	void ShaderFile(Shader* val = NULL) override;

private:
	void OpenModel();
	void InputHandle();
	void InputTransition();
	void InputMove();
	void InputAction();
	void ModelRotation();
	void Play();
	void Combo();
	void DecideAction(D3DXVECTOR3& velocity);
	void DecideValid();
	void WeaponChage(WeaponType type);

	bool Prepare(PlayerAnimation animation);
	bool Priority(PlayerAnimation animation);
	bool Movable(MoveEnd type = MoveEnd::Normal);
	float CorrectionDegree(float& degree);
	float MoveSpeed();
	WeaponType GetWeaponType() { return currentWeaponType; }

private:
	float currentWaistAngle;
	float rootRotationAngle;
	float rootElapsedAngle;

	bool bRun;

	PlayerAnimation prepareAnimation;
	PlayerAnimation currentAnimation;
	PlayerAnimation priorityAnimation;

	WeaponType currentWeaponType;

	class Bar* targetHpBar;
	ExecuteValues* values;
	struct GamePlayerInput* input;
	struct GamePlayerSpec* spec;
};