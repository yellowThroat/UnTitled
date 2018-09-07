#include "stdafx.h"
#include "Character.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Objects/AnimationBlender.h"
#include "../Bounding/Sphere.h"
#include "../Weapons/Weapon.h"

Character::Character()
	: model(NULL)
	, currentWeapon(NULL)
	, mode(Mode::Free)
{
}

Character::~Character()
{
	SAFE_DELETE(model);
	SAFE_DELETE(anim);

	for (auto hurtBox : hurtBoxes)
		SAFE_DELETE(hurtBox);	
	for (auto weapon : weapons)
		SAFE_DELETE(weapon);
}

void Character::Update()
{
	if (anim)
		anim->Update();

	for (auto hurtBox : hurtBoxes)
	{
		D3DXMATRIX mat;
		mat = GetTransform(hurtBox->index);
		hurtBox->box->SetWorld(mat);
		hurtBox->box->Update();
	}

	if (currentWeapon && mode == Mode::Battle)
		currentWeapon->Update();
}

void Character::Render()
{
	if (anim)
		anim->Render();

	for (auto hurtBox : hurtBoxes)
		hurtBox->box->Render();

	if (currentWeapon && mode == Mode::Battle)
		currentWeapon->Render();
}

void Character::Damaged(Character * hitter)
{
	for (UINT i = 0; i < hitter->GetWeapon()->GetHitBoxes().size(); i++)
	{
		for (UINT i = 0; i < hurtBoxes.size(); i++)
		{

		}
	}
}

D3DXMATRIX Character::GetTransform(UINT index)
{
	D3DXMATRIX mat = model->Bone(index)->AbsoluteTransform();
	D3DXMATRIX skin = anim->GetTransform(index);

	return mat * skin;
}