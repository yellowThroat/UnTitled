#include "stdafx.h"
#include "ExeGui.h"

ExeGui::ExeGui(ExecuteValues* values)
	: Execute(values)
{
	Json::Value light = (*values->jsonRoot)["Global Light"];
	if (light.isNull() == false)
	{
		Json::GetValue(light, "Direction", values->GlobalLight->Data.Direction);
		Json::GetValue(light, "Color", values->GlobalLight->Data.Color);
		Json::GetValue(light, "Overcast", values->GlobalLight->Data.Overcast);
		Json::GetValue(light, "Dir", values->GlobalLight->Data.Dir);
	}
}

ExeGui::~ExeGui()
{
	Json::Value light;
	Json::SetValue(light, "Direction", values->GlobalLight->Data.Direction);
	Json::SetValue(light, "Color", values->GlobalLight->Data.Color);
	Json::SetValue(light, "Overcast", values->GlobalLight->Data.Overcast);
	Json::SetValue(light, "Dir", values->GlobalLight->Data.Dir);
	(*values->jsonRoot)["Global Light"] = light;
}

void ExeGui::Update()
{
	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down(VK_F1))
			ChangeShowEnvironmentWindow();

		if (Keyboard::Get()->Down(VK_F2))
			ChangeShowLandscapeWindow();

		if (Keyboard::Get()->Down(VK_F10))
			ChangeShowGlobalLightWindow();

		if (Keyboard::Get()->Down(VK_F11))
			ChangeShowSystemInfoWindow();

		if (Keyboard::Get()->Down(VK_F12))
			ChangeShowDemoWindow();
	}
}

void ExeGui::Render()
{
	
}

void ExeGui::PostRender()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Environment", "Ctrl+F1"))
				ChangeShowEnvironmentWindow();

			if (ImGui::MenuItem("Landscape", "Ctrl+F2"))
				ChangeShowLandscapeWindow();

			if (ImGui::MenuItem("Global Light", "Ctrl+F10"))
				ChangeShowGlobalLightWindow();

			if (ImGui::MenuItem("SystemInfo", "Ctrl+F11"))
				ChangeShowSystemInfoWindow();

			if (ImGui::MenuItem("Demo", "Ctrl+F12"))
				ChangeShowDemoWindow();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	
	D3DXVECTOR3 cameraPosition;
	D3DXVECTOR2 cameraRotation;
	values->MainCamera->GetPosition(&cameraPosition);
	values->MainCamera->GetRotationDegree(&cameraRotation);

	if (values->GuiSettings->bShowSystemInfoWindow == true)
	{
		ImGui::Begin("System Info", &values->GuiSettings->bShowSystemInfoWindow);
		ImGui::Text("Frame Per Second : %4.0f", ImGui::GetIO().Framerate);
		ImGui::Text("Running Time : %4.3f", values->GlobalTime->Data.RunningTime);
		ImGui::Text("Elapsed Time : %4.3f", values->GlobalTime->Data.ElapsedTime);
		ImGui::Separator();
		ImGui::Text
		(
			"Camera Position : %3.0f, %3.0f, %3.0f"
			, cameraPosition.x, cameraPosition.y, cameraPosition.z
		);
		ImGui::Text
		(
			"Camera Rotation : %3.0f, %3.0f", cameraRotation.x, cameraRotation.y
		);
		
		ImGui::End();
	}

	if (values->GuiSettings->bShowGlobalLightWindow == true)
	{
		ImGui::Begin("Global Light", &values->GuiSettings->bShowGlobalLightWindow);
		
		//ImGui::ColorEdit3("Color", values->GlobalLight->Data.Color);
		//ImGui::SliderFloat("Overcast", &values->GlobalLight->Data.Overcast, 0.0001f, 1.0f);
		ImGui::SliderFloat3("Direction", values->GlobalLight->Data.Dir, -1, 1);
		D3DXVec3Normalize(
			&values->GlobalLight->Data.Direction,
			&values->GlobalLight->Data.Dir);
		ImGui::End();
	}

	if (values->GuiSettings->bShowDemoWindow == true)
	{
		ImGui::ShowDemoWindow(&values->GuiSettings->bShowDemoWindow);
	}
}

void ExeGui::ChangeShowGlobalLightWindow()
{
	values->GuiSettings->bShowGlobalLightWindow =
		!values->GuiSettings->bShowGlobalLightWindow;
}

void ExeGui::ChangeShowEnvironmentWindow()
{
	values->GuiSettings->bShowEnvironmentWindow =
		!values->GuiSettings->bShowEnvironmentWindow;
}

void ExeGui::ChangeShowLandscapeWindow()
{
	values->GuiSettings->bShowLandscapeWindow =
		!values->GuiSettings->bShowLandscapeWindow;
}

void ExeGui::ChangeShowSystemInfoWindow()
{
	values->GuiSettings->bShowSystemInfoWindow =
		!values->GuiSettings->bShowSystemInfoWindow;
}

void ExeGui::ChangeShowDemoWindow()
{
	values->GuiSettings->bShowDemoWindow =
		!values->GuiSettings->bShowDemoWindow;
}
