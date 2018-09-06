#include "stdafx.h"
#include "Character.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Objects/AnimationBlender.h"
#include "../Bounding/Sphere.h"

Character::Character()
	: model(NULL)
{
}

Character::~Character()
{
	SAFE_DELETE(model);

	for (auto hurtBox : hurtBoxes)
		SAFE_DELETE(hurtBox);	
}

void Character::Update()
{
	if (anim)
		anim->Update();

	for (auto hitBox : hitBoxes)
		hitBox->Update();
	for (auto hurtBox : hurtBoxes)
		hurtBox->Update();
}

void Character::Render()
{
	if (anim)
		anim->Render();

	for (auto hitBox : hitBoxes)
		hitBox->Render();
	for (auto hurtBox : hurtBoxes)
		hurtBox->Render();
}

void Character::ClearHurtBoxes()
{
	hurtBoxes.clear();
}

void Character::ClearHitBoxes()
{
	hitBoxes.clear();
}
