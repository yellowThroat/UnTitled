#pragma once
#include "Character.h"


class Eve : public Character
{
public:
	enum class PlayerAnimation
	{
		UnKnown = -1,
		Idle, Walk,	Run, 
		Jump,
		Boxing_Idle, Boxing_Step,
		LeadJap, RightHook,
		Count,
	};

	enum class Mode
	{
		Unknown = -1,
		Free, Battle,
		Count,
	};

	enum class MoveEnd
	{
		Unknown = -1,
		Normal, Combo,
		Count,
	};

public:
	Eve(ExecuteValues* values);
	~Eve();
	D3DXVECTOR3 * Target() { return &position; }
	void Update() override;
	

private:
	void OpenModel();
	void InputHandle();
	void InputTransition();
	void InputMove();
	void InputAction();
	void ModelRotation();
	void Play();
	void Combo();
	bool Prepare(PlayerAnimation animation);
	bool Priority(PlayerAnimation animation);
	void DecideAction(D3DXVECTOR3& velocity);
	bool Movable(MoveEnd type = MoveEnd::Normal);
	float CorrectionDegree(float& degree);
	float MoveSpeed();

private:
	float currentWaistAngle;
	float rootRotationAngle;
	float rootElapsedAngle;

	bool bRun;
	Mode mode;

	ExecuteValues* values;
	struct GamePlayerInput* input;
	struct GamePlayerSpec* spec;
	
	
private:

	PlayerAnimation prepareAnimation;
	PlayerAnimation currentAnimation;
	PlayerAnimation priorityAnimation;
};