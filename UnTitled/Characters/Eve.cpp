#include "stdafx.h"
#include "Eve.h"
#include "../Model/ModelAnimPlayer.h"
#include "GamePlayerInput.h"
#include "GameData.h"
#include "../Objects/AnimationBlender.h"
#include "../Objects/AnimationClip.h"

Eve::Eve()
	: moveSpeed(5.0f), bRun(false)
	, currentWaistAngle(0.0f), rootRotationAngle(0.0f), rootElapsedAngle(0.0f)
	, currentAnimation(PlayerAnimation::UnKnown)
	, prepareAnimation(PlayerAnimation::UnKnown)
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

	CalcPosition();
	ModelRotation();

	D3DXMATRIX S, R, T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotate.y, rotate.x, rotate.z);

	world = R * T;

	Character::Update();
}

void Eve::OpenModel()
{
	model = new Model();
	model->ReadMaterial(Models + L"Characters/Player/Eve.material");
	model->ReadMesh(Models + L"Characters/Player/Eve.mesh");
	model->LoadAnimationSet(Models + L"Animation/Player AnimSet.json");

	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 0.05f, 0.05f, 0.05f);
	
	anim = new ModelAnimPlayer(model);
	anim->RootAxis(S);
	anim->World(&world);
}

void Eve::InputHandle()
{
	InputMove();
	if (prepareAnimation != PlayerAnimation::UnKnown)
		Play();
}

void Eve::InputMove()
{	
	if (!Movable()) return;

	velocity = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 moveDirection = D3DXVECTOR3(0, 0, 0);
	if (input->Stroke(GamePlayerKey::Run)) bRun = true;
	if (input->Released(GamePlayerKey::Run)) bRun = false;

	if (input->IsPressMovement())
	{
		float moveSpeed = spec->WalkSpeed;
		if (bRun) moveSpeed = spec->RunSpeed;

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

			if (bRun) moveSpeed = spec->BackRunSpeed;
			else moveSpeed = spec->BackWalkSpeed;
		}
		else
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, 0);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, 0);
		}
		D3DXVec3Normalize(&moveDirection, &moveDirection);
		velocity = moveDirection * moveSpeed;
	}

	DecideAction(moveDirection);

	if (input->Pressed(GamePlayerKey::Jump) && moveDirection.z >= 0)
		Prepare(PlayerAnimation::Jump);
}

void Eve::ModelRotation()
{
	if (input->IsPressMovement())
	{
		if (rootRotationAngle > currentWaistAngle)
			rootElapsedAngle = Math::Clamp(3.0f, 0.0f, rootRotationAngle - currentWaistAngle);
		else if (rootRotationAngle < currentWaistAngle)
			rootElapsedAngle = Math::Clamp(-3.0f, rootRotationAngle - currentWaistAngle, 0.0f);
		else rootElapsedAngle = 0.0f;

		currentWaistAngle += rootElapsedAngle;
		rotate.y = Math::ToRadian(currentWaistAngle);
	}
}

void Eve::Play()
{
	float blendTime = 0.0f;
	bool bLoop = true;
	switch (prepareAnimation)
	{
	case Eve::PlayerAnimation::Idle:
	case Eve::PlayerAnimation::Walk:
	case Eve::PlayerAnimation::BackWardWalk:
	case Eve::PlayerAnimation::Run:
	case Eve::PlayerAnimation::BackWardRun:
	case Eve::PlayerAnimation::TurnLeft:
	case Eve::PlayerAnimation::TurnRight:
		blendTime = 0.3f;
		break;
	case Eve::PlayerAnimation::Jump:
		blendTime = 0.1f;
		bLoop = false;
		break;
	}

	anim->Play((UINT)prepareAnimation, blendTime, bLoop);
	
	currentAnimation = prepareAnimation;
	prepareAnimation = PlayerAnimation::UnKnown;
}

void Eve::Prepare(PlayerAnimation animation)
{
	if (currentAnimation != animation)
		prepareAnimation = animation;
}

void Eve::DecideAction(D3DXVECTOR3 & direction)
{
	bool bMove = true;

	rootRotationAngle = 0.0f;

	if (direction.z > 0.0f)
	{
		if (direction.x < 0.0f)
			rootRotationAngle = -45.0f;
		else if (direction.x > 0.0f)
			rootRotationAngle = 45.0f;
		
		if (bRun)
			Prepare(PlayerAnimation::Run);
		 else Prepare(PlayerAnimation::Walk);
	}
	else if (direction.z < 0.0f)
	{
		if (direction.x < 0.0f)
			rootRotationAngle = 30.0f;
		else if (direction.x > 0.0f)
			rootRotationAngle = -30.0f;	
		
		if (bRun)
			Prepare(PlayerAnimation::BackWardRun);
		else Prepare(PlayerAnimation::BackWardWalk);
	}
	else
	{
		if (direction.x != 0.0f)
		{
			if (direction.x < 0.0f)
				rootRotationAngle = -90.0f;
			else rootRotationAngle = 90.0f;
			
			if (bRun)
				Prepare(PlayerAnimation::Run);
			else Prepare(PlayerAnimation::Walk);

		}
		else bMove = false;
	}

	if (!bMove)
		Prepare(PlayerAnimation::Idle);

}

bool Eve::Movable()
{
	bool b = true;
	switch (currentAnimation)
	{
	case Eve::PlayerAnimation::UnKnown:
		break;
	case Eve::PlayerAnimation::Idle:
	case Eve::PlayerAnimation::Walk:
	case Eve::PlayerAnimation::BackWardWalk:
	case Eve::PlayerAnimation::Run:
	case Eve::PlayerAnimation::BackWardRun:
	case Eve::PlayerAnimation::TurnLeft:
	case Eve::PlayerAnimation::TurnRight:
		b = true;
		break;
	case Eve::PlayerAnimation::Jump:
		b = false;
		break;
	case Eve::PlayerAnimation::Count:
		break;
	}

	if (!b && !anim->NextClip()->clip)
		b |= anim->CurrentClip()->IsDone();
	
	return b;
}
