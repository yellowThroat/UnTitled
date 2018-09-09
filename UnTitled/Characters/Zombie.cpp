#include "stdafx.h"
#include "Zombie.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Bounding/Capsule.h"

Zombie::Zombie()
	: spec(NULL)
{
	spec = new GameZombieSpec();
	OpenModel();
}

Zombie::~Zombie()
{
	SAFE_DELETE(spec);
}

void Zombie::Update()
{
	if (Movable()) Prepare(ZombieAnimation::Idle);

	if (prepareAnimation != ZombieAnimation::Unknown)
		Play();

	Character::Update();
}

void Zombie::Render()
{
	Character::Render();
}

bool Zombie::Damaged(Character * hitter)
{
	if (Character::Damaged(hitter))
	{
		prepareAnimation = ZombieAnimation::Damaged;
		return true;
	}

	return false;
}

void Zombie::OpenModel()
{
	model = new Model();
	model->ReadMaterial(Models + L"Characters/Monsters/Zombie.material");
	model->ReadMesh(Models + L"Characters/Monsters/Zombie.mesh");
	model->ReadAnimation(Models + L"Animation/Zombie Idle.anim");
	model->ReadAnimation(Models + L"Animation/Zombie Reaction Hit.anim");

	D3DXMatrixScaling(&rootAxis, 0.03f, 0.03f, 0.03f);

	anim = new ModelAnimPlayer(model);
	anim->World(&world);
	anim->RootAxis(rootAxis);

	Shapes::BoundingBox* box;
	
	box = new Shapes::BoundingBox();
	box->index = model->Bone(L"Spine1")->Index();
	box->box = new Shapes::Capsule(D3DXVECTOR3(0, -40, 0), D3DXVECTOR3(0, 60, 0), 30.0f);
	hurtBoxes.push_back(box);

}

float Zombie::MoveSpeed()
{
	return 0.0f;
}

bool Zombie::Movable(MoveEnd type)
{
	bool b = true;
	switch (currentAnimation)
	{
	case Zombie::ZombieAnimation::Damaged:
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
		blendTime = 0.2f;
		break;
	case Zombie::ZombieAnimation::Damaged:
		blendTime = 0.1f;
		bLoop = false;
		break;
	}

	anim->Play((UINT)prepareAnimation, blendTime, bLoop, cut);
	currentAnimation = prepareAnimation;
	prepareAnimation = ZombieAnimation::Unknown;
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
