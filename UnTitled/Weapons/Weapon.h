#pragma once

namespace Shapes
{
	class Shape;
	class BoundingBox;
}

class Weapon 
{
public:
	Weapon(class Character* character = NULL);
	~Weapon();

	virtual void Update();
	virtual void Render();
	void SetValid(UINT index, bool val);
	vector<class Shapes::BoundingBox*>& GetHitBoxes() { return hitBoxes; }

protected:
	vector<class Shapes::BoundingBox*> hitBoxes;
	class Character* character;
	D3DXMATRIX rootAxis;
};