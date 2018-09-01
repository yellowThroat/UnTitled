#include "stdafx.h"
#include "Sword.h"

Sword::Sword()
{
	OpenModel();
}

Sword::~Sword()
{
}

void Sword::OpenModel()
{
	model = new Model();
	model->ReadMaterial(Models + L"Weapons/Sword/Sword.material");
	model->ReadMesh(Models + L"Weapons/Sword/Sword.mesh");
}
