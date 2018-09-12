#pragma once

struct GameWeaponSpec
{
	float Atk;
};

struct PlayerFistSpec : public GameWeaponSpec
{
	PlayerFistSpec()
	{
		Atk = 3.0f;
	}
};

struct PlayerSwordSpec : public GameWeaponSpec
{
	PlayerSwordSpec()
	{
		Atk = 20.0f;
	}
};

struct ZombieFistSpec : public GameWeaponSpec
{
	ZombieFistSpec()
	{
		Atk = 3.0f;
	}
};
