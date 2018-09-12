#include "stdafx.h"
#include "Eve.h"
#include "../Model/ModelAnimPlayer.h"
#include "GamePlayerInput.h"
#include "../Bounding/Capsule.h"
#include "../Bounding/Sphere.h"
#include "../Weapons/Fist.h"
#include "../Weapons/Sword.h"
#include "../GUI/Bar.h"

Eve::Eve(ExecuteValues* values)
	: bRun(false)
	, currentWaistAngle(0.0f), rootRotationAngle(0.0f), rootElapsedAngle(0.0f)
	, currentAnimation(PlayerAnimation::UnKnown)
	, prepareAnimation(PlayerAnimation::UnKnown)
	, priorityAnimation(PlayerAnimation::UnKnown)
	, currentWeaponType(WeaponType::UnArmed)
	, values(values)
{
	OpenModel();

	input = new GamePlayerInput();
	targetHpBar = new Bar(Images + L"hpBarBottom.png", Images + L"hpBarTop.png", NULL);
}

Eve::~Eve()
{
	SAFE_DELETE(input);
	SAFE_DELETE(targetHpBar);
}

void Eve::UpdateInGame()
{
	InputHandle();

	ModelRotation();

	CalcPosition();

	D3DXMATRIX R, T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotate.y + Math::ToRadian(currentWaistAngle), rotate.x, rotate.z);

	World(R * T);


	Character::Update();

	DecideValid();

	if (target)
	{
		targetHpBar->SetTarget(target->Target());
		targetHpBar->SetRatio(target->GetHpRatio());

	}
	else
		targetHpBar->SetTarget(NULL);

	targetHpBar->Update();
}

void Eve::Update()
{
	Character::Update();

}

void Eve::Render()
{
	if (target)
	{
		ImGui::Text("target name %s", target->Name().c_str());
		targetHpBar->Render();
	}
	Character::Render();


}

void Eve::OpenModel()
{
	type = RenderType::Player;
	name = "Eve";
	strcpy_s(editBuf.name, name.c_str());

	// Model
	model = new Model();
	model->ReadMaterial(Models + L"Characters/Player/Eve.material");
	model->ReadMesh(Models + L"Characters/Player/Eve.mesh");
	model->LoadAnimationSet(Models + L"Animation/Player AnimSet.json");

	oShader = new Shader(Shaders + L"999_Animation.hlsl");

	for (auto material : model->Materials())
		material->SetShader(oShader);

	// Animation
	D3DXMatrixScaling(&rootAxis, 0.05f, 0.05f, 0.05f);
	anim = new ModelAnimPlayer(model);
	anim->RootAxis(rootAxis);
	anim->World(&world);

	// Bounding Box
	// Hurt Box
	Shapes::BoundingBox* box;

	box = new Shapes::BoundingBox();
	box->index = model->Bone(L"Spine1")->Index();
	box->box = new Shapes::Capsule(D3DXVECTOR3(0, -40, 0), D3DXVECTOR3(0, 30, 0), 20.0f);
	hurtBoxes.push_back(box);

	// Weapon
	weapons.push_back(new Fist(this));
	weapons.push_back(new Sword(this));
	
	WeaponChage(WeaponType::Fist);

	// spec
	basicSpec = new GamePlayerSpec();
	spec = dynamic_cast<GamePlayerSpec*>(basicSpec);

	InitStats();
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

	if (!Movable()) return;

	if (input->Stroke(GamePlayerKey::Transition))
	{
		switch (mode)
		{
		case Eve::Mode::Free:	mode = Mode::Battle; break;
		case Eve::Mode::Battle: mode = Mode::Free; break;
		}

	}

	if (input->isStrokeSlot() >= 0)
		WeaponChage((WeaponType)input->isStrokeSlot());
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
		if (currentAnimation == PlayerAnimation::OneHand_First)
			Priority(PlayerAnimation::OneHand_Second);
		if (currentAnimation == PlayerAnimation::OneHand_Second)
			Priority(PlayerAnimation::OneHand_Last);			
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	if (!Movable(MoveEnd::Normal)) return;

	if (input->Stroke(GamePlayerKey::Jump))
		Prepare(PlayerAnimation::Jump);

	if (input->Stroke(GamePlayerKey::Attack))
	{
		mode = Mode::Battle;
		switch (currentWeaponType)
		{
		case Eve::WeaponType::Fist:
			if (Prepare(PlayerAnimation::OnePunch))
				bStop = true;
			break;
		case Eve::WeaponType::OneHand:
			if (Prepare(PlayerAnimation::OneHand_First))
				bStop = true;
			break;
		}
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
	bool bAttack = false;
	int cut = 0;
	switch (prepareAnimation)
	{
	case Eve::PlayerAnimation::Idle:
	case Eve::PlayerAnimation::Walk:
	case Eve::PlayerAnimation::Run:
	case Eve::PlayerAnimation::Boxing_Idle:
	case Eve::PlayerAnimation::Boxing_Step:
	case Eve::PlayerAnimation::OneHand_Idle:
	case Eve::PlayerAnimation::OneHand_Walk:
	case Eve::PlayerAnimation::OneHand_Run:
		blendTime = 0.2f;
		break;
	case Eve::PlayerAnimation::Jump:
		blendTime = 0.1f;
		bLoop = false;
		break;
	case Eve::PlayerAnimation::Damaged:
		velocity = D3DXVECTOR3(0, 0, 0);
		blendTime = 0.1f;
		bLoop = false;
		break;
	case Eve::PlayerAnimation::OnePunch:
		weapons[(UINT)WeaponType::Fist]->ClearVictim();
		blendTime = 0.1f;
		bLoop = false;
		bAttack = true;
		cut = 12;
		break;
	case Eve::PlayerAnimation::OneHand_First:
		weapons[(UINT)WeaponType::OneHand]->ClearVictim();
		blendTime = 0.1f;
		bLoop = false;
		bAttack = true;
		cut = 12;
		break;
	}

	if (bAttack && target)
	{
		D3DXVECTOR3 forward = Direction();
		D3DXVECTOR3 dir = target->Position() - position;

		float angle = Math::AngleToDegreeY(forward, dir);
		currentWaistAngle += angle;
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
		cut = 18;
		break;
	case PlayerAnimation::Kick:
		blendTime = 0.1f;
		break;
	case PlayerAnimation::OneHand_Second:
		blendTime = 0.1f;
		cut = 18;
		break;
	case PlayerAnimation::OneHand_Last:
		blendTime = 0.1f;
		break;
	}
	velocity = D3DXVECTOR3(0, 0, 0);
	currentWeapon->ClearVictim();

	if (target)
	{
		D3DXVECTOR3 forward = Direction();
		D3DXVECTOR3 dir = target->Position() - position;

		float angle = Math::AngleToDegreeY(forward, dir);
		currentWaistAngle += angle;
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
			switch (currentWeaponType)
			{
			case Eve::WeaponType::Fist:
				Prepare(PlayerAnimation::Boxing_Step);
				break;
			case Eve::WeaponType::OneHand:
				if(bRun)
				Prepare(PlayerAnimation::OneHand_Run);
				else
					Prepare(PlayerAnimation::OneHand_Walk);
				break;
			}
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
			switch (currentWeaponType)
			{
			case Eve::WeaponType::Fist:
				Prepare(PlayerAnimation::Boxing_Idle);
				break;
			case Eve::WeaponType::OneHand:
				Prepare(PlayerAnimation::OneHand_Idle);
				break;
			}
			break;
		}
	}
}

void Eve::DecideValid()
{
	int frame = 0;
	if(!anim->NextClip()->clip)
		frame = anim->CurrentClip()->GetCurrentFrame();

	bool bValid = false;
	UINT index = 0;

	for (UINT i = 0; i < currentWeapon->GetHitBoxes().size(); i++)
		currentWeapon->SetValid(i, false);

	switch (currentAnimation)
	{
	case Eve::PlayerAnimation::OnePunch:
		if (frame >= 4 && frame <= 14)
		{
			bValid = true;
			index = 0;
		}
		break;
	case Eve::PlayerAnimation::TwoPunch:
		if (frame >= 7 && frame <= 13)
		{
			bValid = true;
			index = 1;
		}
		break;
	case Eve::PlayerAnimation::Kick:
		if (frame >= 15 && frame <= 25)
		{
			bValid = true;
			index = 2;
		}
		break;
	case Eve::PlayerAnimation::OneHand_First:
		if (frame >= 18 && frame <= 32)
			bValid = true;
		break;
	case Eve::PlayerAnimation::OneHand_Second:
		if (frame >= 22 && frame <= 27)
			bValid = true;
		break;
	case Eve::PlayerAnimation::OneHand_Last:
		if (frame >= 18 && frame <= 30)
			bValid = true;
		break;
	}

	if (bValid)
		currentWeapon->SetValid(index, true);
}

bool Eve::Movable(MoveEnd type)
{
	bool b = true;
	switch (currentAnimation)
	{
	case Eve::PlayerAnimation::Jump:
	case Eve::PlayerAnimation::OnePunch:
	case Eve::PlayerAnimation::TwoPunch:
	case Eve::PlayerAnimation::Kick:
	case Eve::PlayerAnimation::OneHand_First:
	case Eve::PlayerAnimation::OneHand_Second:
	case Eve::PlayerAnimation::OneHand_Last:
	case Eve::PlayerAnimation::Damaged:
		b = false;
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
		switch (currentWeaponType)
		{
		case Eve::WeaponType::Fist:
			speed = spec->BoxingStepSpeed;
			break;
		case Eve::WeaponType::OneHand:
			if (bRun)
				speed = spec->BattleRunSpeed;
			else speed = spec->BattleWalkSpeed;
			break;
		}
		break;
	}

	return speed;
}

void Eve::WeaponChage(WeaponType type)
{
	if (currentWeaponType == type || (UINT)WeaponType::Count <= (UINT)type) return;

	currentWeaponType = type;
	currentWeapon = NULL;

	if (type == WeaponType::UnArmed) return;

	currentWeapon = weapons[(int)type];
}

bool Eve::Damaged(Character * hitter)
{
	float distance = D3DXVec3Length(&(hitter->Position() - position));
	if (distance >= 5.0f) return false;

	if (Character::Damaged(hitter))
	{
		prepareAnimation = PlayerAnimation::Damaged;
		return true;
	}

	return false;
}

void Eve::SearchTarget(Character * character)
{
	D3DXVECTOR3 forward = Direction();
	D3DXVECTOR3 dir;
	float angle;
	float distance = spec->TargetDistance;
	float newAngle;
	float newDistance;

	if (target)
	{
		dir = target->Position() - position;
		angle = Math::AngleToDegreeY(forward, dir);
		distance = D3DXVec3Length(&(target->Position() - position));

		if (distance >= spec->TargetDistance || fabsf(angle) >= spec->TargetAngle)
			target = NULL;
	}

	if (!character) return;

	dir = character->Position() - position;

	newAngle = Math::AngleToDegreeY(forward, dir);
	newDistance = D3DXVec3Length(&(character->Position() - position));

	if (fabsf(newAngle) < spec->TargetAngle && newDistance < distance)
		target = character;
}

void Eve::ShaderFile(Shader * val)
{
	if (!val)
	{
		for (auto material : model->Materials())
			material->SetShader(oShader);

		return;
	}

	for (auto material : model->Materials())
		material->SetShader(val);
}
