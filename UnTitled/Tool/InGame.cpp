#include "stdafx.h"
#include "InGame.h"
#include "../Lights/DirectionalLight.h"
#include "../Landscape/Terrain.h"
#include "../Characters/Eve.h"
#include "../Characters/Character.h"
#include "../Objects/GameRender.h"

InGame::InGame(ExecuteValues * values)
	: GameScene(values)
	, eve(NULL)
	, terrain(NULL)
	, light(NULL)
{
	light = new DirectionalLight(values);
	shadowBuffer = new ViewProjectionBuffer();
}

InGame::~InGame()
{
	SAFE_DELETE(light);
	SAFE_DELETE(shadowBuffer);
}

void InGame::Update()
{
	light->Update();

	if (eve)
		eve->UpdateInGame();

	for (auto monster : monsters)
	{
		monster->UpdateInGame();
		monster->Damaged(eve);
		eve->SearchTarget(monster);
		eve->Damaged(monster);
	}

	for (UINT i = 0; i < monsters.size();)
	{
		if (monsters[i]->GetExistTime() <= 0.0f)
		{
			if (eve->GetTarget() == monsters[i])
				eve->SetTarget(NULL);
			light->EraseObject(monsters[i]->GetNum());
			SAFE_DELETE(monsters[i]);
			monsters.erase(monsters.begin() + i);
		}
		else i++;
	}
}

void InGame::PreRender()
{
	light->Render();
}

void InGame::Render()
{
	shadowBuffer->SetView(light->View());
	shadowBuffer->SetProjection(light->Projection());
	shadowBuffer->SetVSBuffer(10);

	ID3D11ShaderResourceView* view = light->SRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &view);

	if (terrain)
	{
		terrain->ShaderFile();
		terrain->Render();
	}

	if (eve)
	{
		eve->ShaderFile();
		eve->Render();
	}

	for (auto monster : monsters)
	{
		monster->ShaderFile();
		monster->Render();
	}
}

void InGame::PostRender()
{
}

void InGame::Clear()
{
	light->Clear();
	SAFE_DELETE(eve);
	SAFE_DELETE(terrain);
	for (auto monster : monsters)
		SAFE_DELETE(monster);
}

void InGame::TakeRootInfo()
{
	eve = NULL;
	monsters.clear();

	GameRender * root = GameRender::root;
	for (UINT i = 0; i < root->ChildCount(); i++)
		TakeInfo(root->Child(i));

	for (auto monster : monsters)
		monster->SetTarget(eve);
}

void InGame::TakeInfo(GameRender * object)
{
	GameRender::RenderType type = object->GetType();
	bool bLight = true;
	switch (type)
	{
	case GameRender::RenderType::Player:
		eve = dynamic_cast<Eve*>(object);
		light->Position(eve->Target());
		break;
	case GameRender::RenderType::Zombie:
		monsters.push_back(dynamic_cast<Character*>(object));
		break;
	case GameRender::RenderType::Building:
		break;
	case GameRender::RenderType::Terrain:
		terrain = dynamic_cast<Landscape::Terrain*>(object);
		bLight = false;
		break;
	case GameRender::RenderType::PointLight:
		break;
	case GameRender::RenderType::SpotLight:
		break;
	case GameRender::RenderType::Etc:
		break;
	}

	if (bLight)
		light->AddObject(object);

	for (UINT i = 0; i < object->ChildCount(); i++)
	{
		TakeInfo(object->Child(i));
	}
}

D3DXVECTOR3 * InGame::Target()
{
	if (eve)
		return eve->Target();

	return NULL;
}
