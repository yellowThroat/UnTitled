#pragma once
#include "stdafx.h"

enum class GamePlayerKey
{
	MoveForward = 0, MoveBackward, MoveLeft, MoveRight,
	Transition,
	Run, Jump, Attack,
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

		keyboard[(UINT)GamePlayerKey::Transition]	= 'X';
		keyboard[(UINT)GamePlayerKey::Attack]		= VK_LBUTTON;

		keyboard[(UINT)GamePlayerKey::Run] = VK_LSHIFT;
		keyboard[(UINT)GamePlayerKey::Jump]	= VK_SPACE;
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
