#include "stdafx.h"
#include "Weapon.h"
#include "WeaponDatas.h"
#include "../Bounding/Shape.h"
#include "../Characters/Character.h"

Weapon::Weapon(Character* character)
	: character(character)
	, spec(NULL)
{
	D3DXMatrixIdentity(&rootAxis);
}

Weapon::~Weapon()
{
	for (auto hitBox : hitBoxes)
		SAFE_DELETE(hitBox);

	SAFE_DELETE(spec);
}

void Weapon::Update()
{
	for (UINT i = 0; i < hitBoxes.size(); i++)
	{
		D3DXMATRIX mat;
		mat = character->GetTransform(hitBoxes[i]->index);
		hitBoxes[i]->box->SetWorld(mat);
		if (hitBoxes[i]->valid)
			hitBoxes[i]->box->SetColor(D3DXCOLOR(1, 0, 0, 1));
		else
			hitBoxes[i]->box->SetColor(D3DXCOLOR(0, 0, 1, 1));

		if (hitBoxes[i]->model)
		{
			vector<D3DXMATRIX> transforms;
			hitBoxes[i]->model->CopyAbsoluteBoneTo(rootAxis * mat, transforms);
			for (ModelMesh* mesh : hitBoxes[i]->model->Meshes())
				mesh->SetWorld(transforms[i]);
		}

	}
}

void Weapon::PreRender()
{
	for (auto model : hitBoxes)
		if (model->model)
			model->model->Mesh(0)->RenderWithoutShader();
}

void Weapon::Render()
{
	for (UINT i = 0; i < hitBoxes.size(); i++)
	{
		hitBoxes[i]->box->Render();
		if (hitBoxes[i]->model)
			hitBoxes[i]->model->Mesh(0)->Render();
	}
}

void Weapon::SetValid(UINT index, bool val)
{
	hitBoxes[index]->valid = val;
}

void Weapon::AddVictim(Character * victim)
{
	victims.push_back(victim);
}

void Weapon::ClearVictim()
{
	victims.clear();
}

bool Weapon::CheckVictim(Character * victim)
{
	for (auto v : victims)
	{
		if (v == victim) return true;
	}

	return false;
}

float Weapon::GetWeaponAtk()
{
	return spec->Atk;
}
