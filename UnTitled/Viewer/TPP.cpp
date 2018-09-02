#include "stdafx.h"
#include "TPP.h"

TPP::TPP(float rotationSpeed)
	: rotationSpeed(rotationSpeed)
{

}

TPP::~TPP()
{

}

void TPP::Update()
{	
	D3DXVECTOR2 rotation;
	GetRotation(&rotation);

	D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

	rotation.x += move.y * rotationSpeed;
	rotation.y += move.x * rotationSpeed;

	rotation.x = Math::Clamp(rotation.x, -(float)D3DX_PI / 2.0f, (float)D3DX_PI / 2.0f);
	

	SetRotation(rotation.x, rotation.y);	
}
