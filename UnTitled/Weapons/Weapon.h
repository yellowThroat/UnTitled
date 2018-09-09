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
	void AddVictim(Character* victim);
	void ClearVictim();
	bool CheckVictim(Character* victim);
protected:
	vector<class Character*> victims;
	vector<class Shapes::BoundingBox*> hitBoxes;
	class Character* character;
	D3DXMATRIX rootAxis;
};