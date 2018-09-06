#include "stdafx.h"
#include "Zombie.h"

Zombie::Zombie()
	: spec(NULL)
{
	spec = new GameZombieSpec();
}

Zombie::~Zombie()
{
	SAFE_DELETE(spec);
}

void Zombie::Update()
{
}

void Zombie::Render()
{
}
