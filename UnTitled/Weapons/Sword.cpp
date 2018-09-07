#include "stdafx.h"
#include "Sword.h"
#include "../Characters/Character.h"
#include "../Bounding/Capsule.h"

Sword::Sword(Character* character)
	: Weapon(character)
{
	OpenModel();
}

Sword::~Sword()
{
}

void Sword::Update()
{
	Weapon::Update();
}

void Sword::OpenModel()
{
	Shapes::BoundingBox* hitBox;

	hitBox = new Shapes::BoundingBox();
	hitBox->index = character->GetModel()->Bone(L"RightHand")->Index();
	hitBox->model = new Model();
	hitBox->model->ReadMaterial(Models + L"Weapons/Sword/Sword.material");
	hitBox->model->ReadMesh(Models + L"Weapons/Sword/Sword.mesh");
	hitBox->box = new Shapes::Capsule(D3DXVECTOR3(8.7f, -2.4f, 0.0f), D3DXVECTOR3(8.7f, -2.4f, 58.0f), 3.0f);

	hitBoxes.push_back(hitBox);

	D3DXMatrixTranslation(&rootAxis, 13.7f, -3.4f, 25.0f);
}
