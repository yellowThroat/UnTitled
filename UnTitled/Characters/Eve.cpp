#include "stdafx.h"
#include "Eve.h"
#include "../Model/ModelAnimPlayer.h"
#include "GamePlayerInput.h"
#include "GameData.h"
#include "../Objects/AnimationBlender.h"
#include "../Objects/AnimationClip.h"

Eve::Eve(ExecuteValues* values)
	: bRun(false)
	, currentWaistAngle(0.0f), rootRotationAngle(0.0f), rootElapsedAngle(0.0f)
	, currentAnimation(PlayerAnimation::UnKnown)
	, prepareAnimation(PlayerAnimation::UnKnown)
	, priorityAnimation(PlayerAnimation::UnKnown)
	, mode(Mode::Free)
	, values(values)
{
	OpenModel();

	spec = new GamePlayerSpec();
	input = new GamePlayerInput();
}

Eve::~Eve()
{
	SAFE_DELETE(spec);
	SAFE_DELETE(input);
}

void Eve::Update()
{
	InputHandle();

	ModelRotation();

	D3DXMATRIX S, R, T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotate.y + Math::ToRadian(currentWaistAngle), rotate.x, rotate.z);

	World(R * T);

	CalcPosition();

	Character::Update();
}

void Eve::OpenModel()
{
	model = new Model();
	model->ReadMaterial(Models + L"Characters/Player/Eve.material");
	model->ReadMesh(Models + L"Characters/Player/Eve.mesh");
	model->LoadAnimationSet(Models + L"Animation/Player AnimSet.json");

	D3DXMATRIX S;
	D3DXMatrixScaling(&S, -0.05f, 0.05f, 0.05f);
	
	anim = new ModelAnimPlayer(model);
	anim->RootAxis(S);
	anim->World(&world);

	headHurtIndex = model->Bone(L"Head")->Index();
	bodyHurtIndex = model->Bone(L"Hips")->Index();
}

void Eve::InputHandle()
{
	InputTransition();
	InputMove();
	InputAction();

	if (priorityAnimation != PlayerAnimation::UnKnown &&
		Movable(MoveEnd::Combo))
		Combo();

	if (prepareAnimation != PlayerAnimation::UnKnown)
		Play();

}

void Eve::InputTransition()
{
	if (input->Stroke(GamePlayerKey::Run)) bRun = true;
	if (input->Released(GamePlayerKey::Run)) bRun = false;
	if (input->Stroke(GamePlayerKey::Transition))
	{
		switch (mode)
		{
		case Eve::Mode::Free:	mode = Mode::Battle; break;
		case Eve::Mode::Battle: mode = Mode::Free; break;
		}
	}
}

void Eve::InputMove()
{	
	if (!Movable(MoveEnd::Normal)) return;

	velocity = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 moveDirection = D3DXVECTOR3(0, 0, 0);

	if (input->IsPressMovement())
	{
		if (input->Pressed(GamePlayerKey::MoveForward))
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, 1);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, 1);
			else moveDirection = D3DXVECTOR3(0, 0, 1);
		}
		else if (input->Pressed(GamePlayerKey::MoveBackward))
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, -1);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, -1);
			else moveDirection = D3DXVECTOR3(0, 0, -1);
		}
		else
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, 0);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, 0);
		}
		D3DXVec3Normalize(&moveDirection, &moveDirection);
		velocity = D3DXVECTOR3(0, 0, 1) * MoveSpeed();
	}

	DecideAction(moveDirection);

}

void Eve::InputAction()
{
	bool bStop = false;

	if (input->Stroke(GamePlayerKey::Attack))
	{
		if (currentAnimation == PlayerAnimation::OnePunch)
			Priority(PlayerAnimation::TwoPunch);
		if (currentAnimation == PlayerAnimation::TwoPunch)
			Priority(PlayerAnimation::Kick);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	if (!Movable(MoveEnd::Normal)) return;

	if (input->Stroke(GamePlayerKey::Jump))
		Prepare(PlayerAnimation::Jump);

	if (input->Stroke(GamePlayerKey::Attack))
	{
		if (Prepare(PlayerAnimation::OnePunch))
			bStop = true;
	}

	if (bStop)
		velocity = D3DXVECTOR3(0, 0, 0);
}

void Eve::ModelRotation()
{
	if (!Movable()) return;
	if (input->IsPressMovement())
	{
		if (rootRotationAngle > 0.0f)
			rootElapsedAngle = Math::Clamp(spec->RotateSpeed, 0.0f, rootRotationAngle);
		else if (rootRotationAngle < 0.0f)
			rootElapsedAngle = Math::Clamp(-spec->RotateSpeed, rootRotationAngle, 0.0f);
		else rootElapsedAngle = 0.0f;

		currentWaistAngle += rootElapsedAngle;
	}
}

void Eve::Play()
{
	float blendTime = 0.0f;
	bool bLoop = true;
	int cut = 0;
	switch (prepareAnimation)
	{
	case Eve::PlayerAnimation::Idle:
	case Eve::PlayerAnimation::Walk:
	case Eve::PlayerAnimation::Run:
	case Eve::PlayerAnimation::Boxing_Idle:
	case Eve::PlayerAnimation::Boxing_Step:
		blendTime = 0.2f;
		break;
	case Eve::PlayerAnimation::Jump:
		blendTime = 0.1f;
		bLoop = false;
		break;
	case Eve::PlayerAnimation::OnePunch:
		blendTime = 0.1f;
		bLoop = false;
		cut = 12;
		break;
	}
	
	anim->Play((UINT)prepareAnimation, blendTime, bLoop, cut);
	currentAnimation = prepareAnimation;
	prepareAnimation = PlayerAnimation::UnKnown;
}

void Eve::Combo()
{
	float blendTime = 0.0f;
	bool bLoop = false;
	int cut = 0;

	switch (priorityAnimation)
	{
	case PlayerAnimation::TwoPunch:
		blendTime = 0.1f;
		bLoop = false;
		cut = 18;
		break;
	case PlayerAnimation::Kick:
		blendTime = 0.1f;
		bLoop = false;
		break;
	}

	anim->Play((UINT)priorityAnimation, blendTime, bLoop, cut);

	currentAnimation = priorityAnimation;
	priorityAnimation = PlayerAnimation::UnKnown;
	prepareAnimation = PlayerAnimation::UnKnown;
}

bool Eve::Prepare(PlayerAnimation animation)
{
	if (currentAnimation != animation)
	{
		prepareAnimation = animation;
		return true;
	}

	return false;
}

bool Eve::Priority(PlayerAnimation animation)
{
	if (currentAnimation != animation && priorityAnimation == PlayerAnimation::UnKnown)
	{
		priorityAnimation = animation;
		return true;
	}

	return false;
}

void Eve::DecideAction(D3DXVECTOR3 & direction)
{
	bool bMove = true;

	rootRotationAngle = 0.0f;

	D3DXVECTOR3 d, forward;
	forward = Direction();
	values->MainCamera->GetDirection(d); d.y = 0;

	D3DXVec3Normalize(&forward, &forward);
	D3DXVec3Normalize(&d, &d);

	float angle = D3DXVec3Dot(&forward, &d);
	angle = Math::Clamp(angle, -1.0f, 1.0f);
	angle = acosf(angle);

	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &forward, &d);
	if (cross.y < 0) angle = -angle;
	angle = Math::ToDegree(angle);

	if (direction.z > 0.0f)
	{
		if (direction.x < 0.0f)
			angle -= 45.0f;
		else if (direction.x > 0.0f)
			angle += 45.0f;

		rootRotationAngle = CorrectionDegree(angle);

	}
	else if (direction.z < 0.0f)
	{
		angle += 180.0f;
		if (direction.x < 0.0f)
			angle += 45.0f;
		else if (direction.x > 0.0f)
			angle -= 45.0f;

		rootRotationAngle = CorrectionDegree(angle);
	}
	else
	{
		if (direction.x < 0.0f)
			angle -= 90.0f;
		else if(direction.x > 0.0f)
			angle += 90.0f;
		rootRotationAngle = CorrectionDegree(angle);

		if (direction.x == 0.0f)
		{
			bMove = false;
			rootRotationAngle = 0.0f;
		}
	}

	if (bMove)
	{
		switch (mode)
		{
		case Eve::Mode::Free:
			if (bRun)
				Prepare(PlayerAnimation::Run);
			else Prepare(PlayerAnimation::Walk);
			break;
		case Eve::Mode::Battle:
			Prepare(PlayerAnimation::Boxing_Step);
			break;
		}
	}
	else
	{
		switch (mode)
		{
		case Eve::Mode::Free:
			Prepare(PlayerAnimation::Idle);
			break;
		case Eve::Mode::Battle:
			Prepare(PlayerAnimation::Boxing_Idle);
			break;
		}
	}
}

bool Eve::Movable(MoveEnd type)
{
	bool b = true;
	switch (currentAnimation)
	{
	case Eve::PlayerAnimation::UnKnown:
		break;
	case Eve::PlayerAnimation::Idle:
	case Eve::PlayerAnimation::Walk:
	case Eve::PlayerAnimation::Run:
	case Eve::PlayerAnimation::Boxing_Idle:
	case Eve::PlayerAnimation::Boxing_Step:
		b = true;
		break;
	case Eve::PlayerAnimation::Jump:
	case Eve::PlayerAnimation::OnePunch:
	case Eve::PlayerAnimation::TwoPunch:
	case Eve::PlayerAnimation::Kick:
		b = false;
		break;
	case Eve::PlayerAnimation::Count:
		break;
	}

	if (!b)
	{
		switch (type)
		{
		case Eve::MoveEnd::Normal:
			if (!anim->NextClip()->clip)
				b |= anim->CurrentClip()->IsDone();
			break;

		case Eve::MoveEnd::Combo:
			if (!anim->NextClip()->clip)
				b |= anim->CurrentClip()->Combo();
			break;
		}
	}
	
	return b;
}

float Eve::CorrectionDegree(float & degree)
{
	if (degree > 180.0f)
		degree -= 360.0f;

	if (degree < -180.0f)
		degree += 360.0f;

	return degree;
}

float Eve::MoveSpeed()
{
	float speed = 0.0f;
	switch (mode)
	{
	case Eve::Mode::Free:
		if (!bRun) speed = spec->WalkSpeed;
		else speed = spec->RunSpeed;
		break;
	case Eve::Mode::Battle:
		speed = spec->BattleMoveSpeed;
		break;
	}

	return speed;
}
