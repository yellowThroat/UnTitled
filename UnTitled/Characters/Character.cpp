#include "stdafx.h"
#include "Character.h"
#include "../Model/ModelAnimPlayer.h"

Character::Character()
	: model(NULL), anim(NULL)
{
}

Character::~Character()
{
	SAFE_DELETE(model);
	SAFE_DELETE(anim);
}

void Character::Update()
{
	if (anim)
		anim->Update();
}

void Character::Render()
{
	if (anim)
		anim->Render();
}

D3DXMATRIX Character::GetBoneTransform(UINT index)
{
	D3DXMATRIX absoluteTransform = model->Bone(index)->AbsoluteTransform();
	return absoluteTransform * anim->GetTransform(index);
}

D3DXMATRIX Character::GetBoneTransform(wstring name)
{
	D3DXMATRIX absoluteTransform = model->Bone(name)->AbsoluteTransform();

	UINT index = model->Bone(name)->Index();
	return absoluteTransform * anim->GetTransform(index);
}
