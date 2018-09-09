#include "stdafx.h"
#include "DrawModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameAnimationModel.h"
#include "../Tool/GameSettings.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Environment/Rain.h"
#include "../Environment/Snow.h"
#include "../Characters/Eve.h"
#include "../Characters/Zombie.h"
#include "../Weapons/Sword.h"
#include "../Bounding/Capsule.h"
#include "../Bounding/Sphere.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
	, settings(NULL)
	, eve(NULL)
	, zombie(NULL)
{
	settings = new GameSettings(values);
	eve = new Eve(values);
	zombie = new Zombie();
}

DrawModel::~DrawModel()
{	
	SAFE_DELETE(zombie);
	SAFE_DELETE(eve);
	SAFE_DELETE(settings);	
}

void DrawModel::Update()
{
	if (settings)
		settings->Update();	

	eve->Update();
	zombie->Update();
	Test();
}

void DrawModel::PreRender()
{
}

void DrawModel::Render()
{
	if (settings)
		settings->Render();
	eve->Render();
	zombie->Render();
}

void DrawModel::PostRender()
{
	if (settings)
		settings->PostRender();
}

void DrawModel::Test()
{
	if (Keyboard::Get()->Down(VK_RETURN))
	{
		values->MainCamera = Camera::cameras[1];
		values->MainCamera->CopyInfo(Camera::cameras[0]);
		values->MainCamera->SetTarget(eve->Target());
	}

	if (Keyboard::Get()->Down('P'))
	{
		values->MainCamera = Camera::cameras[0];
		values->MainCamera->CopyInfo(Camera::cameras[1]);
	}

	zombie->Damaged(eve);
}
