#include "stdafx.h"
#include "Eve.h"
#include "../Model/ModelAnimPlayer.h"
#include "GamePlayerInput.h"
#include "GameData.h"
Eve::Eve()
	: moveSpeed(5.0f), bRun(false)
{
	OpenModel();

	spec = new GamePlayerSpec();
	input = new GamePlayerInput();
}

Eve::~Eve()
{
	SAFE_DELETE(spec);
	SAFE_DELETE(input);
}

void Eve::Update()
{
	InputHandle();

	CalcPosition();

	D3DXMatrixTranslation(&world, position.x, position.y, position.z);

	Character::Update();
}

void Eve::OpenModel()
{
	model = new Model();
	model->ReadMaterial(Models + L"Characters/Player/Eve.material");
	model->ReadMesh(Models + L"Characters/Player/Eve.mesh");
	model->ReadAnimation(Models + L"Characters/Player/Eve.anim");

	anim = new ModelAnimPlayer(model);

	anim->World(&world);
}

void Eve::InputHandle()
{
	InputMove();
}

void Eve::InputMove()
{	
	velocity = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 moveDirection = D3DXVECTOR3(0, 0, 0);

	if (input->IsPressMovement())
	{
		float moveSpeed = spec->WalkSpeed;

		if (bRun) moveSpeed = spec->RunSpeed;

		if (input->Pressed(GamePlayerKey::MoveForward))
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, 1);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, 1);
			else moveDirection = D3DXVECTOR3(0, 0, 1);
		}
		else if (input->Pressed(GamePlayerKey::MoveBackward))
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, -1);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, -1);
			else moveDirection = D3DXVECTOR3(0, 0, -1);
		}
		else
		{
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection = D3DXVECTOR3(-1, 0, 0);
			else if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection = D3DXVECTOR3(1, 0, 0);
		}
		D3DXVec3Normalize(&moveDirection, &moveDirection);
		velocity = moveDirection * moveSpeed;
	}

}
