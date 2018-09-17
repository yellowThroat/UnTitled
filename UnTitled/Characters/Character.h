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

	virtual void UpdateInGame();
	virtual void Update();
	void PreRender() override;
	virtual void Render();
	virtual void PostRender();
	virtual void InitStats();
	virtual bool Damaged(Character* hitter);
	virtual bool BodyCheck(Character* other);
	virtual float MoveSpeed() { return 0.0f; }

public:
	Model* GetModel() { return model; }
	D3DXMATRIX GetTransform(UINT index);

	class Weapon* GetWeapon() { return currentWeapon; }
	void SetTarget(Character* target) { this->target = target; }
	Character* GetTarget() { return target; }
	D3DXVECTOR3 * Target() { return &position; }
	float GetHpRatio() { return currentHp / maxHp; }
	float GetExistTime() { return existTime; }
	Shapes::BoundingBox* GetBody() { return hurtBoxes[0]; }

protected:
	Model* model;
	class ModelAnimPlayer* anim;
	Character* target;

	struct GameCharacterSpec * basicSpec;
	vector<Shapes::BoundingBox*> hurtBoxes;
	vector<class Weapon*> weapons;
	class Weapon* currentWeapon;
	Mode mode;

protected:
	// Stats
	float maxHp;
	float currentHp;
	float existTime;
	bool bAlive;
};