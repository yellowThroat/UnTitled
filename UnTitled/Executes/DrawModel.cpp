#include "stdafx.h"
#include "DrawModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameAnimationModel.h"
#include "../Tool/GameSettings.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Environment/Rain.h"
#include "../Environment/Snow.h"
#include "../Characters/Eve.h"
#include "../Weapons/Sword.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
	, settings(NULL)
{
	settings = new GameSettings(values);
	eve = new Eve(values);
}

DrawModel::~DrawModel()
{	
	SAFE_DELETE(eve);
	SAFE_DELETE(settings);	
}

void DrawModel::Update()
{
	if (settings)
		settings->Update();	

	Test();

	eve->Update();
}

void DrawModel::PreRender()
{
}

void DrawModel::Render()
{
	if (settings)
		settings->Render();
	eve->Render();
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

}
