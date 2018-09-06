#include "stdafx.h"
#include "Weapon.h"
#include "../Bounding/Shape.h"
#include "../Characters/Character.h"

Weapon::Weapon(Character* character)
	: character(character)
{
	D3DXMatrixIdentity(&rootAxis);
}

Weapon::~Weapon()
{
	for (auto hitBox : hitBoxes)
		SAFE_DELETE(hitBox);
}

void Weapon::Update()
{
	for (UINT i = 0; i < hitBoxes.size(); i++)
	{
		D3DXMATRIX mat;
		mat = character->GetTransform(hitBoxes[i]->index);
		hitBoxes[i]->box->SetWorld(mat);
		if (hitBoxes[i]->model)
		{
			vector<D3DXMATRIX> transforms;
			hitBoxes[i]->model->CopyAbsoluteBoneTo(rootAxis * mat, transforms);
			for (ModelMesh* mesh : hitBoxes[i]->model->Meshes())
				mesh->SetWorld(transforms[i]);
		}
	}
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