#pragma once
#include "../Objects/GameRender.h"
#include "GameData.h"

namespace Shapes
{
	class Shape;
	class Sphere;
	class BoundingBox;
}

class Character : public GameRender
{
public:
	enum class Mode
	{
		Unknown = -1,
		Free, Battle,
		Count,
	};

	enum class MoveEnd
	{
		Unknown = -1,
		Normal, Combo,
		Count,
	};


public:
	Character();
	virtual ~Character();

	virtual void Update();
	virtual void Render();

	void Damaged(Character* hitter);

public:
	Model* GetModel() { return model; }
	D3DXMATRIX GetTransform(UINT index);

	class Weapon* GetWeapon() { return currentWeapon; }

protected:
	Model* model;
	class ModelAnimPlayer* anim;


	vector<Shapes::BoundingBox*> hurtBoxes;
	vector<class Weapon*> weapons;
	class Weapon* currentWeapon;
	Mode mode;
};