#pragma once
#include "Weapon.h"
namespace Shapes
{
	class Capsule;
}
class Sword : public Weapon
{
public:
	Sword(class Character* character);
	~Sword();

	void Update() override;
	void Render() override;

private:
	void OpenModel();
	Shapes::Capsule* capsule;
	class WeaponTrail* trail;
};