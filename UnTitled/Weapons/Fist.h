#pragma once
#include "Weapon.h"

class Fist : public Weapon
{
public:
	Fist(class Character* character);
	~Fist();

	void Update() override;

private:
	void OpenModel();

};