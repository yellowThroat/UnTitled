#include "stdafx.h"
#include "Character.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Objects/AnimationBlender.h"

Character::Character()
	: model(NULL)
{
}

Character::~Character()
{
	SAFE_DELETE(model);
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