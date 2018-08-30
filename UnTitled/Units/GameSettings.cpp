#include "stdafx.h"
#include "GameSettings.h"
#include "../Utilities/BinaryFile.h"
#include "../Landscape/Terrain.h"
#include "../Commands/Command.h"
#include "../Lights/PointLight.h"
#include "../Lights/SpotLight.h"
#include "../Lights/ProjectionTexture.h"
#include "../Lights/DirectionalLight.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshTeapot.h"

GameSettings::GameSettings(ExecuteValues* values)
	: fixCamera(false), current(NULL), values(values)
{
	Landscape::Terrain::brush = new Texture(Contents + L"Images/brush.png");
	GameRender::root = new GameRender();
	GameRender::root->SetValues(values);

	GameRender::URManager = new UndoRedoManager();
}

GameSettings::~GameSettings()
{
	SAFE_DELETE(GameRender::URManager);
	SAFE_DELETE(GameRender::root);
	SAFE_DELETE(Landscape::Terrain::brush);
}

void GameSettings::Update()
{
	FixedCamera();
	InputHandle();
	for (UINT i = 0; i < GameRender::root->ChildCount(); i++)
	{
		GameRender::root->Child(i)->Update();
	}
}

void GameSettings::PreRender()
{
}

void GameSettings::Render()
{
	for (UINT i = 0; i < GameRender::root->ChildCount(); i++)
	{
		GameRender::root->Child(i)->Render();
	}
}

void GameSettings::PostRender()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Project"))
			{
				D3DDesc desc;
				D3D::GetDesc(&desc);

				function<void(wstring)> f;
				f = bind(&GameSettings::SaveProject, this, placeholders::_1);

				Path::SaveFileDialog(L"", L"*.*", Project, f, desc.Handle);

			}

			if (ImGui::MenuItem("Load Project"))
			{
				D3DDesc desc;
				D3D::GetDesc(&desc);

				function<void(wstring)> f;
				f = bind(&GameSettings::LoadProject, this, placeholders::_1);

				Path::OpenFileDialog(L"", L"*.pro", Project, f, desc.Handle);
			}

			if (ImGui::MenuItem("Close"))
			{
				GameRender::URManager->Clear();
				GameRender::root->ClearChilds();
				current= NULL;
			}

			ImGui::Separator();


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl + Z"))
				GameRender::URManager->Undo();
			if(ImGui::MenuItem("Redo", "Ctrl + Y"))
				GameRender::URManager->Redo();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::Begin("Hierarchy");

	for (UINT i = 0; i < GameRender::root->ChildCount(); i++)
	{
		GameRender::root->Child(i)->Selectable(&current);
	}

	if (ImGui::IsMouseReleased(0))
	{
		if (!ImGui::IsAnyItemHovered())
		{
			if (GameRender::bParenting)
			{
				GameRender::URManager->AddState(cParenting::Create(current));
				GameRender::root->AddChild(current);
			}
			else if (ImGui::IsMouseHoveringWindow())
			{
				current = NULL;
			}
		}

		GameRender::bParenting = false;
	}
	if (ImGui::IsMouseClicked(1))	
		if (ImGui::IsMouseHoveringWindow())
			ImGui::OpenPopup("Add Object");

	if (ImGui::BeginPopup("Add Object"))
	{
		ImGui::Text("Add Object");
		ImGui::Separator();

		if (ImGui::Selectable("Create Empty"))
			CreateEmpty();

		if (ImGui::BeginMenu("Structure"))
		{

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lights"))
		{
			if (ImGui::MenuItem("Point Light"))
				CreateLights(0);
			if (ImGui::MenuItem("Spot Light"))
				CreateLights(1);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Sphere"))
				CreateObject(0);

			if (ImGui::MenuItem("Cube"))
				CreateObject(1);
			
			if (ImGui::MenuItem("Teapot"))
				CreateObject(2);
			ImGui::EndMenu();
		}

		if (ImGui::Selectable("Terrain"))
			CreateTerrain();

		ImGui::EndPopup();
	}
	

	ImGui::End();

	if (current)
		current->PostRender();

}

void GameSettings::CreateEmpty()
{
	GameRender* object = new GameRender();

	GameRender::URManager->AddState(cNew::Create(object));

	GameRender::root->AddChild(object);
}

void GameSettings::CreateEnvironment()
{
	//GameRender* environment = NULL;

	//GameRender::URManager->AddState(cNew::Create(environment);

	//GameRender::root->AddChild(environment);
}


void GameSettings::CreatePlayer()
{

	//GameRender::URManager->AddState(cNew::Create(player));

	//GameRender::root->AddChild(player);
}

void GameSettings::CreateTerrain()
{
	Landscape::Terrain * terrain = new Landscape::Terrain(values);

	GameRender::URManager->AddState(cNew::Create(terrain));

	GameRender::root->AddChild(terrain);
}

void GameSettings::CreateLights(UINT type)
{
	GameRender* light = NULL;
	switch (type)
	{
	case 0:
		light = new PointLight();
		break;

	case 1:
		light = new SpotLight();
		break;
	}
	GameRender::URManager->AddState(cNew::Create(light));
	GameRender::root->AddChild(light);
}

void GameSettings::CreateObject(UINT type)
{
	GameRender* object = NULL;
	switch (type)
	{
	case 0:	object = new MeshSphere();	break;
	case 1:	object = new MeshCube();	break;
	case 2: object = new MeshTeapot();	break;
	}
	GameRender::URManager->AddState(cNew::Create(object));
	GameRender::root->AddChild(object);
}

void GameSettings::FixedCamera()
{
	if (Keyboard::Get()->Down(VK_RETURN))
		fixCamera = !fixCamera;

	if (!fixCamera) return;

	//D3DXVECTOR3 position = player->Position();
	//D3DXVECTOR3 direction = player->Direction();
	//D3DXVECTOR3 camera = position - 5 * direction;

	//float theta = D3DXVec3Dot(&D3DXVECTOR3(0, 0, 1), &direction);
	//theta = acosf(theta);

	//D3DXVECTOR3 cross;
	//D3DXVec3Cross(&cross, &D3DXVECTOR3(0, 0, 1), &direction);
	//if (cross.y < 0) theta = -theta;

	//values->MainCamera->SetPosition(camera.x, 4.2f, camera.z);
	//values->MainCamera->SetRotation((float)D3DX_PI / 18.0f, theta);
}

void GameSettings::InputHandle()
{
	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down('Z'))
		{
			GameRender::URManager->Undo();

			if (current && current->Parent() == NULL)
				current = NULL;
		}
		if (Keyboard::Get()->Down('Y'))
		{
			GameRender::URManager->Redo();
			if (current && current->Parent() == NULL)
				current = NULL;
		}
	}

	if (Keyboard::Get()->Down(VK_DELETE))
	{
		if (current)
		{
			current->Separation();
			GameRender::URManager->AddState(cDelete::Create(current));
			current = NULL;
		}
	}
}

void GameSettings::DeleteCurrent()
{
	current->Separation();
	SAFE_DELETE(current);
	current = NULL;
}

void GameSettings::SaveProject(wstring file)
{
	wstring saveFolder = Path::GetDirectoryName(file);
	if (Path::ExistDirectory(saveFolder) == false)
		CreateDirectory(saveFolder.c_str(), NULL);
	wstring saveFile = Path::GetFileNameWithoutExtension(file);
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveFile + L".pro");

	GameRender::root->Save(w);

	w->Close();

	SAFE_DELETE(w);
}

void GameSettings::LoadProject(wstring file)
{
	if (file.length() < 1) return;
	GameRender::URManager->Clear();
	GameRender::root->ClearChilds();
	current = NULL;

	BinaryReader* r = new BinaryReader();
	r->Open(file);
	r->UInt();
	GameRender::root->Load(r);

	r->Close();

	SAFE_DELETE(r);
}