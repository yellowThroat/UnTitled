#pragma once
#include "Weapon.h"

class ZombieFist : public Weapon
{
public:
	ZombieFist(class Character* character);
	~ZombieFist();

	void Update() override;

private:
	void OpenModel();
};