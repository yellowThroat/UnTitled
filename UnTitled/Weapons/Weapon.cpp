#include "stdafx.h"
#include "Weapon.h"

Weapon::Weapon()
	: model(NULL)
{
	D3DXMatrixIdentity(&world);
}

Weapon::~Weapon()
{
}

void Weapon::Update()
{
}

void Weapon::Render()
{
	vector<D3DXMATRIX> transforms;
	model->CopyAbsoluteBoneTo(world, transforms);

	for (ModelMesh* mesh : model->Meshes())
	{
		int index = mesh->ParentBoneIndex();
		D3DXMATRIX transform = transforms[index];

		mesh->SetWorld(transform);
		mesh->Render();
	}
}
