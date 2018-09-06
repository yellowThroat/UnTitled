#pragma once
#include "Weapon.h"
class Sword : public Weapon
{
public:
	Sword(class Character* character);
	~Sword();

	void Update() override;

private:
	void OpenModel();
};