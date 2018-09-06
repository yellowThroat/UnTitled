#include "stdafx.h"
#include "TPP.h"

TPP::TPP(float rotationSpeed, float distance)
	: rotationSpeed(rotationSpeed)
	, distance(distance)
{

}

TPP::~TPP()
{

}

void TPP::Update()
{
	D3DXVECTOR3 direction = D3DXVECTOR3(0, 0, 1);
	D3DXVECTOR3 pos = *target;
	GetDirection(direction);
	pos.y = 5.0f;
	pos -= distance * direction;

	SetPosition(pos.x, pos.y, pos.z);


	D3DXVECTOR2 rotation;
	GetRotation(&rotation);

	D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

	FixMouse();

	rotation.x += move.y * rotationSpeed;
	rotation.y += move.x * rotationSpeed;

	rotation.x = Math::Clamp(rotation.x, -(float)D3DX_PI / 2.0f, (float)D3DX_PI / 2.0f);
	

	SetRotation(rotation.x, rotation.y);	
}

void TPP::SetTarget(D3DXVECTOR3 * target)
{
	this->target = target;
}

void TPP::FixMouse()
{
	bool change = false;
	RECT windowRc;
	D3DDesc desc;
	D3D::GetDesc(&desc);
	POINT mousePos;
	POINT correctionPos = { 0, 0 };
	POINT center = { 0, 0 };
	LONG offset = 150;

	GetCursorPos(&mousePos);
	correctionPos.x = mousePos.x;
	correctionPos.y = mousePos.y;

	GetWindowRect(desc.Handle, &windowRc);

	center.x = (LONG)((float)(windowRc.left + windowRc.right) * 0.5f);
	center.y = (LONG)((float)(windowRc.top + windowRc.bottom) * 0.5f);

	if (mousePos.x >= windowRc.right - offset || mousePos.x <= windowRc.left + offset)
	{
		correctionPos.x = center.x;
		change = true;
	}
	if (mousePos.y <= windowRc.top + offset || mousePos.y >= windowRc.bottom - offset)
	{
		correctionPos.y = center.y;
		change = true;
	}

	if (change)
		Mouse::Get()->SetPosition(correctionPos);

}
