#include "stdafx.h"
#include "Character.h"
#include "GameData.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Bounding/Sphere.h"
#include "../Bounding/Shape.h"
#include "../Weapons/Weapon.h"

Character::Character()
	: model(NULL)
	, currentWeapon(NULL)
	, mode(Mode::Free)
	, target(NULL)
	, currentHp(0)
	, maxHp(0)
	, basicSpec(NULL)
	, bAlive(true)
	, existTime(5.0f)
{
	isBoneBuffer->Data.isBone = 2;
}

Character::~Character()
{
	SAFE_DELETE(model);
	SAFE_DELETE(anim);
	SAFE_DELETE(basicSpec);

	for (auto hurtBox : hurtBoxes)
		SAFE_DELETE(hurtBox);	
	for (auto weapon : weapons)
		SAFE_DELETE(weapon);


}

void Character::UpdateInGame()
{
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

void Character::PreRender()
{
	isBoneBuffer->SetVSBuffer(4);

	if (anim)
		anim->Render();
}

void Character::Render()
{
	isBoneBuffer->SetVSBuffer(4);

	if (anim)
		anim->Render();

	for (auto hurtBox : hurtBoxes)
		hurtBox->box->Render();

	if (currentWeapon && mode == Mode::Battle)
		currentWeapon->Render();
}

void Character::PostRender()
{
	GameRender::PostRender();

	if (anim)
		anim->PostRender();
}

void Character::InitStats()
{
	currentHp = maxHp = basicSpec->HealthPoint;
}

bool Character::Damaged(Character * hitter)
{
	for (auto hit : hitter->GetWeapon()->GetHitBoxes())
	{
		for (auto hurt : hurtBoxes)
		{
			if (!hitter->GetWeapon()->CheckVictim(this) && 
				hit->valid &&
				hit->box->Collide(hurt->box))
			{
				currentHp -= hitter->GetWeapon()->GetWeaponAtk();
				if (currentHp < 0)
				{
					currentHp = 0.0f;
					bAlive = false;
				}
				hitter->GetWeapon()->AddVictim(this);
				return true;
			}
		}
	}

	return false;
}

D3DXMATRIX Character::GetTransform(UINT index)
{
	D3DXMATRIX mat = model->Bone(index)->AbsoluteTransform();
	D3DXMATRIX skin = anim->GetTransform(index);

	return mat * skin;
}