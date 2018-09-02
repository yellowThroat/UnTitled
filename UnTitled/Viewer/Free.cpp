#include "stdafx.h"
#include "Free.h"

Free::Free(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{

}

Free::~Free()
{

}

void Free::Update()
{
	if (Mouse::Get()->Press(1) == false)
		return;

	D3DXVECTOR3 forward, right, up;
	GetForward(&forward);
	GetRight(&right);
	GetUp(&up);

	//Translation
	{
		D3DXVECTOR3 position;
		GetPosition(&position);



		if (Keyboard::Get()->Press('W'))
			position += forward * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position += -forward * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('A'))
			position += -right * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			position += right * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			position += up * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			position += -up * moveSpeed * Time::Delta();

		SetPosition(position.x, position.y, position.z);
	}

	//Rotation
	{
		D3DXVECTOR2 rotation;
		GetRotation(&rotation);

		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

			rotation.x += move.y * rotationSpeed * Time::Delta();
			rotation.y += move.x * rotationSpeed * Time::Delta();

			rotation.x = Math::Clamp(rotation.x, -(float)D3DX_PI / 2.0f, (float)D3DX_PI / 2.0f);
		}

		SetRotation(rotation.x, rotation.y);
	}
}
