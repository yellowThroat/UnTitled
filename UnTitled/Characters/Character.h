#pragma once
#include "../Objects/GameRender.h"

class Character : public GameRender
{
public:
	Character();
	virtual ~Character();

	virtual void Update();
	virtual void Render();

protected:
	Model* model;

	class ModelAnimPlayer* anim;
};