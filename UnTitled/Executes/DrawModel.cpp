#include "stdafx.h"
#include "DrawModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameAnimationModel.h"
#include "../Units/GameSettings.h"
#include "../Model/ModelAnimPlayer.h"
#include "../Environment/Rain.h"
#include "../Environment/Snow.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
	, settings(NULL), rain(NULL), snow(NULL)
	, weather(0)
{
	settings = new GameSettings(values);

	WeatherCube Cube = WeatherCube
	(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(100, 100, 100)
	);

	rain = new Rain(Cube, 300, values);
	snow = new Snow(Cube, 300, values);
}

DrawModel::~DrawModel()
{	
	SAFE_DELETE(rain);
	SAFE_DELETE(snow);
	SAFE_DELETE(settings);	
}

void DrawModel::Update()
{
	if (settings)
		settings->Update();	
}

void DrawModel::PreRender()
{
	if (settings)
		settings->PreRender();
}

void DrawModel::Render()
{
	if (settings)
		settings->Render();

	if (rain && weather == 1)
		rain->Render();
	if (snow && weather == 2)
		snow->Render();
}

void DrawModel::PostRender()
{
	if (settings)
		settings->PostRender();

	if (values->GuiSettings->bShowEnvironmentWindow)
	{
		ImGui::Begin("Environment");

		if (ImGui::CollapsingHeader("Weather"))
		{
			ImGui::Combo("Weather ", &weather, "Sunny\0Rainy\0Snowy\0");
			if (rain && weather == 1)
				rain->PostRender();
			if (snow && weather == 2)
				snow->PostRender();
		}

		ImGui::End();
	}
}
