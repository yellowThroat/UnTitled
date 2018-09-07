#pragma once
#include "stdafx.h"

enum class GamePlayerKey
{
	MoveForward = 0, MoveBackward, MoveLeft, MoveRight,
	Transition,
	Run, Jump, Attack,
	FirstSlot, SecondSlot, ThirdSlot, ForthSlot, FifthSlot,
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

		keyboard[(UINT)GamePlayerKey::FirstSlot]	= '1';
		keyboard[(UINT)GamePlayerKey::SecondSlot]	= '2';
		keyboard[(UINT)GamePlayerKey::ThirdSlot]	= '3';
		keyboard[(UINT)GamePlayerKey::ForthSlot]	= '4';
		keyboard[(UINT)GamePlayerKey::FifthSlot]	= '5';

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

	int isStrokeSlot()
	{
		int r = -1;

		if (Stroke(GamePlayerKey::FirstSlot))
			r = 0;

		if (Stroke(GamePlayerKey::SecondSlot))
			r = 1;

		if (Stroke(GamePlayerKey::ThirdSlot))
			r = 2;

		if (Stroke(GamePlayerKey::ForthSlot))
			r = 3;

		if (Stroke(GamePlayerKey::FifthSlot))
			r = 4;

		return r;
	}
};
