#pragma once
#include "stdafx.h"

enum class GamePlayerKey
{
	MoveForward = 0, MoveBackward, MoveLeft, MoveRight,
	TurnLeft, TurnRight,
	WeaponFire, WeaponReload, WeaponChange,
	Boost, 
	Count,
};

struct GamePlayerInput
{
private:
	UINT keyboard[(UINT)GamePlayerKey::Count];

public:
	GamePlayerInput()
	{
		keyboard[(UINT)GamePlayerKey::MoveForward]	= 'W';
		keyboard[(UINT)GamePlayerKey::MoveBackward] = 'S';
		keyboard[(UINT)GamePlayerKey::MoveLeft]		= 'A';
		keyboard[(UINT)GamePlayerKey::MoveRight]	= 'D';

		keyboard[(UINT)GamePlayerKey::TurnLeft]		= 'F';
		keyboard[(UINT)GamePlayerKey::TurnRight]	= 'H';

		keyboard[(UINT)GamePlayerKey::WeaponFire]	= 'G';
		keyboard[(UINT)GamePlayerKey::WeaponReload] = 'R';
		keyboard[(UINT)GamePlayerKey::WeaponChange] = 'V';
		keyboard[(UINT)GamePlayerKey::Boost]	= VK_SPACE;
	}
	void SetGameKey(GamePlayerKey key, UINT setKey)
	{
		keyboard[(UINT)key] = setKey;
	}

	bool Pressed(GamePlayerKey key)
	{
		bool  b = true;
		b &= Mouse::Get()->Press(1) == false;
		b &= Keyboard::Get()->Press(keyboard[(UINT)key]);

		return b;
	}

	bool Released(GamePlayerKey key)
	{
		bool  b = true;
		b &= Mouse::Get()->Press(1) == false;
		b &= Keyboard::Get()->Up(keyboard[(UINT)key]);

		return b;
	}

	bool Stroke(GamePlayerKey key)
	{
		bool  b = true;
		b &= Mouse::Get()->Press(1) == false;
		b &= Keyboard::Get()->Down(keyboard[(UINT)key]);

		return b;
	}

	bool IsPressTurn()
	{
		bool b = false;
		b |= Pressed(GamePlayerKey::TurnLeft);
		b |= Pressed(GamePlayerKey::TurnRight);

		return b;
	}

	bool IsPressMovement()
	{
		bool b = false;
		b |= Pressed(GamePlayerKey::MoveBackward);
		b |= Pressed(GamePlayerKey::MoveForward);
		b |= Pressed(GamePlayerKey::MoveLeft);
		b |= Pressed(GamePlayerKey::MoveRight);

		return b;
	}
};
