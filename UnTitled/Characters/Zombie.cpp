#include "stdafx.h"
#include "Zombie.h"
#include "AiContext.h"
#include "AiState.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Bounding/Capsule.h"
#include "../Weapons/ZombieFist.h"

Zombie::Zombie()
	: spec(NULL)
	, currentState(ZombieState::Wander)
	, currentAnimation(ZombieAnimation::Idle)
	, prepareAnimation(ZombieAnimation::Unknown)
	, initialPosition(D3DXVECTOR3(0, 0, 0))
{
	OpenModel();
	MakeAi();
}

Zombie::~Zombie()
{
	SAFE_DELETE(ai);
}

void Zombie::UpdateInGame()
{
	if (!bAlive)
	{
		existTime -= Time::Delta();
		if (existTime < 0) existTime = 0.0f;

		if (prepareAnimation != ZombieAnimation::Unknown)
			Play();

		Character::Update();

		return;
	}

	ai->Update();

	if (prepareAnimation != ZombieAnimation::Unknown)
		Play();

	ModelRotation();

	CalcPosition();

	D3DXMATRIX R, T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	D3DXMatrixRotationYawPitchRoll(&R, Math::ToRadian(rotate.y), Math::ToRadian(rotate.x), Math::ToRadian(rotate.z));

	World(R * T);

	Character::Update();
	
	DecideValid();
}

void Zombie::Update()
{
	Character::Update();
}

void Zombie::Render()
{
	ImGui::Text("CurrentHp : %f", currentHp);
	Character::Render();
}

bool Zombie::Damaged(Character * hitter)
{
	float distance = D3DXVec3Length(&(hitter->Position() - position));

	if (distance >= 10.0f) return false;

	if (Character::Damaged(hitter))
	{
		if (bAlive)
			prepareAnimation = ZombieAnimation::Damaged;
		else
			prepareAnimation = ZombieAnimation::Death;
		return true;
	}

	return false;
}

void Zombie::ShaderFile(Shader * val)
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

void Zombie::OpenModel()
{
	type = RenderType::Zombie;
	name = "Zombie";
	strcpy_s(editBuf.name, name.c_str());

	model = new Model();
	model->ReadMaterial(Models + L"Characters/Monsters/Zombie.material");
	model->ReadMesh(Models + L"Characters/Monsters/Zombie.mesh");
	model->LoadAnimationSet(Models + L"Animation/Zombie AnimSet.json");

	oShader = new Shader(Shaders + L"999_Animation.hlsl");

	for (auto material : model->Materials())
		material->SetShader(oShader);


	D3DXMatrixScaling(&rootAxis, 0.03f, 0.03f, 0.03f);

	anim = new ModelAnimPlayer(model);
	anim->World(&world);
	anim->RootAxis(rootAxis);

	Shapes::BoundingBox* box;
	
	box = new Shapes::BoundingBox();
	box->index = model->Bone(L"Spine1")->Index();
	box->box = new Shapes::Capsule(D3DXVECTOR3(0, -40, 0), D3DXVECTOR3(0, 60, 0), 30.0f);
	hurtBoxes.push_back(box);

	weapons.push_back(new ZombieFist(this));
	currentWeapon = weapons[0];

	basicSpec = new GameZombieSpec();
	spec = dynamic_cast<GameZombieSpec*>(basicSpec);

	InitStats();
}

void Zombie::MakeAi()
{
	ai = new AiContext();

	AiState* state = NULL;
	
	state = new AiState();
	state->Updating = bind(&Zombie::OnAiSearch, this);
	ai->AddState(L"Search", state);

	state = new AiState();
	state->Updating = bind(&Zombie::OnAiMove, this);
	ai->AddState(L"Move", state);

	state = new AiState();
	state->Updating = bind(&Zombie::OnAiAttack, this);
	ai->AddState(L"Attack", state);

	ai->Play((UINT)ZombieAiType::Search);
	ai->Play((UINT)ZombieAiType::Move);
}

void Zombie::ModelRotation()
{
	float angle = spec->RotateSpeed;
	if (currentState == ZombieState::Wander)
		angle *= 0.1f;

	if (rootRotationAngle > 0.0f)
		elapsedAngle = Math::Clamp(angle, 0.0f, rootRotationAngle);
	else if (rootRotationAngle < 0.0f)
		elapsedAngle = Math::Clamp(-angle, rootRotationAngle, 0);

	rotate.y += elapsedAngle;
}

float Zombie::MoveSpeed()
{
	float speed;

	if (!bRun)
		speed = spec->WalkSpeed;
	else speed = spec->RunSpeed;

	return speed;
}

void Zombie::OnAiMove()
{
	if (!Movable()) return;

	mode = Mode::Free;

	switch (currentState)
	{
	case Zombie::ZombieState::Wander:
		bRun = false;
		Prepare(ZombieAnimation::Wander);
		break;
	case Zombie::ZombieState::Return:
	case Zombie::ZombieState::Found:
		bRun = true;
		Prepare(ZombieAnimation::Run);
		break;
	}

	velocity = MoveSpeed() * D3DXVECTOR3(0, 0, 1);
}

void Zombie::OnAiSearch()
{
	velocity = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 targetPosition = D3DXVECTOR3(0, 0, 0);

	float moveRange = D3DXVec3Length(&(position - initialPosition));
	float distance = D3DXVec3Length(&(position - target->Position()));
	bool onTarget = false;

	if (currentState == ZombieState::Return)
	{
		mode = Mode::Free;
		if (moveRange <= 1.0f)
			currentState = ZombieState::Wander;
	}

	if (currentState != ZombieState::Return)
	{
		if (moveRange >= spec->MoveRange)
			currentState = ZombieState::Return;
		else if(distance <= spec->SearchRange)
		{
			currentState = ZombieState::Found;
		}
		else
		{
			currentState = ZombieState::Wander;
		}
	}

	if (distance >= spec->attackRange && !ai->FindState((UINT)ZombieAiType::Move)->IsActive())
	{
		ai->Play((UINT)ZombieAiType::Move);
		ai->Stop((UINT)ZombieAiType::Attack);
	}
	else if (distance < spec->attackRange && currentState != ZombieState::Attack)
	{
		ai->Stop((UINT)ZombieAiType::Move);
		ai->Play((UINT)ZombieAiType::Attack);
		currentState = ZombieState::Attack;
	}

	switch (currentState)
	{
	case Zombie::ZombieState::Wander:
		break;

	case Zombie::ZombieState::Found:
	case Zombie::ZombieState::Attack:
		targetPosition = target->Position();
		onTarget = true;
		break;

	case Zombie::ZombieState::Return:
		targetPosition = initialPosition;
		onTarget = true;
		break;
	}

	if (onTarget)
	{
		D3DXVECTOR3 forward = Direction();
		D3DXVECTOR3 dir = targetPosition - position;

		rootRotationAngle = Math::AngleToDegreeY(forward, dir);
	}
	else
	{
		rootRotationAngle += Math::Random(-1.0f, 1.0f);
	}
}

void Zombie::OnAiAttack()
{
	if (!Movable()) return;

	velocity = D3DXVECTOR3(0, 0, 0);
	
	prepareAnimation = ZombieAnimation::Attack;

	mode = Mode::Battle;
}

bool Zombie::Movable(MoveEnd type)
{
	bool b = true;
	switch (currentAnimation)
	{
	case Zombie::ZombieAnimation::Damaged:
	case Zombie::ZombieAnimation::Attack:
	case Zombie::ZombieAnimation::Death:
		b = false;
		break;
	}

	if (!b)
	{
		switch (type)
		{
		case Character::MoveEnd::Normal:
			if (!anim->NextClip()->clip)
				b |= anim->CurrentClip()->IsDone();
			break;
		case Character::MoveEnd::Combo:
			if (!anim->NextClip()->clip)
				b |= anim->CurrentClip()->Combo();
			break;
		}
	}
	return b;
}

void Zombie::Play()
{
	float blendTime = 0.0f;
	bool bLoop = true;
	int cut = 0;
	switch (prepareAnimation)
	{
	case Zombie::ZombieAnimation::Idle:
	case Zombie::ZombieAnimation::Wander:
	case Zombie::ZombieAnimation::Run:
		blendTime = 0.2f;
		break;
	case Zombie::ZombieAnimation::Damaged:
	case Zombie::ZombieAnimation::Death:
		blendTime = 0.2f;
		bLoop = false;
		break;
	case Zombie::ZombieAnimation::Attack:
		blendTime = 0.2f;
		bLoop = false;
		currentWeapon->ClearVictim();
		break;
	}

	anim->Play((UINT)prepareAnimation, blendTime, bLoop, cut);
	currentAnimation = prepareAnimation;
	prepareAnimation = ZombieAnimation::Unknown;
}

void Zombie::DecideValid()
{
	int frame = 0;
	if (!anim->NextClip()->clip)
		frame = anim->CurrentClip()->GetCurrentFrame();

	bool bValid = false;
	UINT index = 0;

	for (UINT i = 0; i < currentWeapon->GetHitBoxes().size(); i++)
		currentWeapon->SetValid(i, false);

	switch (currentAnimation)
	{
	case Zombie::ZombieAnimation::Attack:
		if (frame >= 24 && frame <= 35)
		{
			bValid = true;
			index = 0;
		}
		break;
	}

	if (bValid)
		currentWeapon->SetValid(index, true);
}

bool Zombie::Prepare(ZombieAnimation animation)
{
	if (currentAnimation != animation)
	{
		prepareAnimation = animation;
		return true;
	}
	return false;
}
