#include "stdafx.h"
#include "Fist.h"
#include "../Bounding/Sphere.h"
#include "../Characters/Character.h"

Fist::Fist(Character* character)
	: Weapon(character)
{
	OpenModel();
}

Fist::~Fist()
{
}

void Fist::Update()
{
	Weapon::Update();
	
}

void Fist::OpenModel()
{
	Shapes::BoundingBox* hitBox;

	hitBox = new Shapes::BoundingBox();
	hitBox->index = character->GetModel()->Bone(L"RightHand")->Index();
	hitBox->box = new Shapes::Sphere(10.0f);
	hitBox->box->SetColor(D3DXCOLOR(0, 0, 1, 1));
	hitBoxes.push_back(hitBox);

	hitBox = new Shapes::BoundingBox();
	hitBox->index = character->GetModel()->Bone(L"LeftHand")->Index();
	hitBox->box = new Shapes::Sphere(10.0f);
	hitBox->box->SetColor(D3DXCOLOR(0, 0, 1, 1));
	hitBoxes.push_back(hitBox);

	hitBox = new Shapes::BoundingBox();
	hitBox->index = character->GetModel()->Bone(L"RightFoot")->Index();
	hitBox->box = new Shapes::Sphere(10.0f);
	hitBox->box->SetColor(D3DXCOLOR(0, 0, 1, 1));
	hitBoxes.push_back(hitBox);

	hitBox = new Shapes::BoundingBox();
	hitBox->index = character->GetModel()->Bone(L"LeftFoot")->Index();
	hitBox->box = new Shapes::Sphere(10.0f);
	hitBox->box->SetColor(D3DXCOLOR(0, 0, 1, 1));
	hitBoxes.push_back(hitBox);
}
