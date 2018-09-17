#include "stdafx.h"
#include "Sword.h"
#include "WeaponDatas.h"
#include "../Characters/Character.h"
#include "../Bounding/Capsule.h"
#include "../Bounding/Segment.h"
#include "../Effects/WeaponTrail.h"

Sword::Sword(Character* character)
	: Weapon(character)
{
	OpenModel();

	trail = new WeaponTrail(100, 25);
}

Sword::~Sword()
{
	SAFE_DELETE(trail);
}

void Sword::Update()
{
	Weapon::Update();
	trail->Update();
	D3DXVECTOR3 bottom = capsule->GetSegment()->p0;
	D3DXVECTOR3 top = capsule->GetSegment()->p1;
	trail->SetData(bottom, top);
}

void Sword::Render()
{
	Weapon::Render();

	trail->Render();
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
	capsule = dynamic_cast<Shapes::Capsule*>(hitBox->box);
	hitBoxes.push_back(hitBox);

	D3DXMatrixTranslation(&rootAxis, 13.7f, -3.4f, 25.0f);

	spec = new PlayerSwordSpec();

}
