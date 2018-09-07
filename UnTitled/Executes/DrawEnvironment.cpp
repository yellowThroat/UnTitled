#include "stdafx.h"

#include "DrawEnvironment.h"
#include "../Environment/Sky.h"
#include "../Environment/SkyPlane.h"
#include "../Landscape/Grid.h"

DrawEnvironment::DrawEnvironment(ExecuteValues * values)
	: Execute(values)
	, sky(NULL), grid(NULL)
{
	sky = new Sky(values);
	grid = new Grid();
}

DrawEnvironment::~DrawEnvironment()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(grid);
}

void DrawEnvironment::Update()
{
	if (sky)
	{
		D3DXVECTOR3 p;
		values->MainCamera->GetPosition(&p);
		sky->Position(p);
		sky->Update();
	}


}

void DrawEnvironment::PreRender()
{
}

void DrawEnvironment::Render()
{
	if (sky)
		sky->Render();

	if (grid)
		grid->Render();
}

void DrawEnvironment::PostRender()
{
	if (values->GuiSettings->bShowEnvironmentWindow)
	{
		ImGui::Begin("Environment");

		if (sky)
			sky->PostRender();


		ImGui::End();
	}
}