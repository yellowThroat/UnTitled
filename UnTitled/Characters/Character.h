#pragma once
#include "../Objects/GameRender.h"

namespace Shapes
{
	class Shape;
	class Sphere;
}

class Character : public GameRender
{
public:
	Character();
	virtual ~Character();

	virtual void Update();
	virtual void Render();

protected:
	void ClearHurtBoxes();
	void ClearHitBoxes();

protected:
	Model* model;

	class ModelAnimPlayer* anim;

	vector<Shapes::Shape*> hurtBoxes;
	vector<Shapes::Shape*> hitBoxes;
};