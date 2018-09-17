#include "stdafx.h"
#include "DrawModel.h"
#include "../Tool/GameSettings.h"
#include "../Tool/InGame.h"
#include "../Characters/Eve.h"
#include "../GUI/Canvas.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
	, settings(NULL)
	, inGame(NULL)
	, currentScene(NULL)
	, currentSceneType(SceneType::LevelEditor)
{
	settings = new GameSettings(values);
	inGame = new InGame(values);
	currentScene = settings;
}

DrawModel::~DrawModel()
{	
	SAFE_DELETE(settings);	
	SAFE_DELETE(inGame);
}

void DrawModel::Update()
{
	InputHandle();

	if (currentScene)
		currentScene->Update();
}

void DrawModel::PreRender()
{
	if (currentScene)
		currentScene->PreRender();
}

void DrawModel::Render()
{
	currentScene->Render();
}

void DrawModel::PostRender()
{
	currentScene->PostRender();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Run"))
		{
			if (ImGui::MenuItem("Runs", "Tap"))
			{
				RunGame();
			}

			if (ImGui::MenuItem("Stop", "Tap"))
			{
				RunEditor();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void DrawModel::InputHandle()
{
	if (Keyboard::Get()->Down(VK_TAB))
	{
		if (Keyboard::Get()->Press(VK_MENU)) return;

		switch (currentSceneType)
		{
		case DrawModel::SceneType::LevelEditor:
			RunGame();
			break;
		case DrawModel::SceneType::InGame:
			RunEditor();
			break;
		}
	}
}

void DrawModel::RunGame()
{
	settings->SaveProject(Project + L"Temp");
	inGame->TakeRootInfo();
	currentSceneType = SceneType::InGame;
	currentScene = inGame;
	//values->MainCamera = Camera::cameras[1];
	//values->MainCamera->SetTarget(inGame->Target());
	GameRender::root->NullChilds();;
}

void DrawModel::RunEditor()
{
	currentSceneType = SceneType::LevelEditor;
	currentScene = settings;
	values->MainCamera = Camera::cameras[0];
	values->MainCamera->CopyInfo(Camera::cameras[1]);

	inGame->Clear();
	settings->LoadProject(Project + L"Temp.pro");
}

