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
	Character::Update();
}

void Zombie::Render()
{
	Character::Render();
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
	box->box = new Shapes::Capsule(D3DXVECTOR3(0, -40, 0), D3DXVECTOR3(0, 60, 0), 20.0f);
	hurtBoxes.push_back(box);

}

float Zombie::MoveSpeed()
{
	return 0.0f;
}

bool Zombie::Movable(MoveEnd type)
{
	return false;
}
