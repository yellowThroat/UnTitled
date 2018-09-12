#include "stdafx.h"
#include "ZombieFist.h"
#include "WeaponDatas.h"
#include "../Bounding/Sphere.h"
#include "../Characters/Character.h"

ZombieFist::ZombieFist(Character* character)
	: Weapon(character)
{
	OpenModel();
}

ZombieFist::~ZombieFist()
{
}

void ZombieFist::Update()
{
	Weapon::Update();	
}

void ZombieFist::OpenModel()
{
	Shapes::BoundingBox* hitBox;

	hitBox = new Shapes::BoundingBox();
	hitBox->index = character->GetModel()->Bone(L"RightHand")->Index();
	hitBox->box = new Shapes::Sphere(10.0f);
	hitBox->box->SetColor(D3DXCOLOR(0, 0, 1, 1));
	hitBoxes.push_back(hitBox);

	spec = new ZombieFistSpec();
}
