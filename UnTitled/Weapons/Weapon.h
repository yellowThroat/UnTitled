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

protected:
	vector<class Shapes::BoundingBox*> hitBoxes;
	class Character* character;
	D3DXMATRIX rootAxis;
};