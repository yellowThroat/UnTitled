#include "stdafx.h"
#include "GameRender.h"
#include "../Utilities/BinaryFile.h"
#include "../Landscape/Terrain.h"
#include "../Commands/Command.h"
#include "../Lights/PointLight.h"
#include "../Lights/SpotLight.h"

UINT GameRender::numOfRender = 0;
bool GameRender::bParenting = false;
GameRender* GameRender::root = NULL;
UndoRedoManager* GameRender::URManager = NULL;

GameRender::GameRender()
	: enable(true), visible(true)
	, name("No Name"), oShader(NULL), parent(NULL)
	, position(0, 0, 0), rotate(0, 0, 0), scale(1, 1, 1)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
	, velocity(0, 0, 0)
	, type(RenderType::Unknown)
	, isBoneBuffer(NULL)
{
	D3DXMatrixIdentity(&rootAxis);
	D3DXMatrixIdentity(&world);
	
	num = numOfRender++;
	strcpy_s(editBuf.name, name.c_str());

	editBuf.position = position;
	editBuf.rotate = rotate;
	editBuf.scale = scale;
	isBoneBuffer = new IsBoneBuffer();
}

GameRender::~GameRender()
{
	SAFE_DELETE(isBoneBuffer);
	SAFE_DELETE(oShader);
	for (GameRender* render : childs)
		SAFE_DELETE(render);
}

void GameRender::Enable(bool val)
{
	enable = val;

	for(auto temp : Enabled)
		temp();
}

bool GameRender::Enable()
{
	return enable;
}

void GameRender::Visible(bool val)
{
	visible = val;

	for (auto temp : Visibled)
		temp();
}

bool GameRender::Visible()
{
	return visible;
}

void GameRender::RootAxis(D3DXMATRIX & matrix)
{
	rootAxis = matrix;
}

void GameRender::RootAxis(D3DXVECTOR3 & rotate)
{
	D3DXMatrixRotationYawPitchRoll(
		&rootAxis,
		Math::ToRadian(rotate.y),
		Math::ToRadian(rotate.x),
		Math::ToRadian(rotate.z)
	);
}

D3DXMATRIX GameRender::RootAxis()
{
	return rootAxis;
}

void GameRender::World(D3DXMATRIX & matrix)
{
	world = matrix;

	position = D3DXVECTOR3(world._41, world._42, world._43);

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);

}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::Position(D3DXVECTOR3 & vec)
{
	position = vec;

	world._41 = vec.x;
	world._42 = vec.y;
	world._43 = vec.z;
}

D3DXVECTOR3 GameRender::Position()
{
	return position;
}

void GameRender::Direction(D3DXVECTOR3 & vec)
{
	direction = vec;

	world._31 = vec.x;
	world._32 = vec.y;
	world._33 = vec.z;
}

D3DXVECTOR3 GameRender::Direction()
{
	return direction;
}

void GameRender::Up(D3DXVECTOR3 & vec)
{
	up = vec;

	world._21 = vec.x;
	world._22 = vec.y;
	world._23 = vec.z;
}

D3DXVECTOR3 GameRender::Up()
{
	return up;
}

void GameRender::Right(D3DXVECTOR3 & vec)
{
	right = vec;

	world._11 = right.x;
	world._12 = right.y;
	world._13 = right.z;
}

D3DXVECTOR3 GameRender::Right()
{
	return right;
}

void GameRender::Rotation(D3DXVECTOR3 & vec)
{	
	rotate = vec;
	D3DXVECTOR3 radian;
	radian.x = Math::ToRadian(rotate.x);
	radian.y = Math::ToRadian(rotate.y);
	radian.z = Math::ToRadian(rotate.z);
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, radian.y, radian.x, radian.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	World(S * R * T);
}

D3DXVECTOR3 GameRender::Rotation()
{
	return rotate;
}

void GameRender::Scaling(D3DXVECTOR3 & vec)
{
	scale = vec;
	D3DXVECTOR3 radian;
	radian.x = Math::ToRadian(rotate.x);
	radian.y = Math::ToRadian(rotate.y);
	radian.z = Math::ToRadian(rotate.z);

	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, radian.y, radian.x, radian.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	World(S * R * T);
}

D3DXVECTOR3 GameRender::Scaling()
{
	return scale;
}

void GameRender::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameRender::Velocity()
{
	return velocity;
}

D3DXVECTOR3 GameRender::CalcVelocity(D3DXVECTOR3 & velocity)
{
	D3DXVECTOR3 v(0, 0, 0);

	if (velocity.z != 0.0f)
		v += Direction() * velocity.z;

	if (velocity.x != 0.0f)
		v += Right() * velocity.x;

	if (velocity.y != 0.0f)
		v += Up() * velocity.y;

	return v;
}

void GameRender::AddPosition(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 pos = Position() + vec;

	Position(pos);
}

void GameRender::CalcPosition()
{
	if (D3DXVec3Length(&velocity) > 0)
	{
		D3DXVECTOR3 v = CalcVelocity(velocity) * Time::Delta();

		AddPosition(v);
	}
}

D3DXMATRIX GameRender::Transformed()
{
	return rootAxis * world;
}

void GameRender::Update()
{
	for (GameRender* render : childs)
		render->Update();
}

void GameRender::Render()
{
	for (GameRender* render : childs)
		render->Render();	
}

void GameRender::PostRender()
{
	ImGui::Begin("Inspector");
	ImGui::Text(name.c_str());

	if (ImGui::BeginPopupContextItem("Edit name"))
	{
		ImGui::InputText("Edit", editBuf.name, 32);

		ImGui::EndPopup();
	}
	else if (name != editBuf.name)
	{
		URManager->AddState(cString::Create(this, name));
		name = editBuf.name;
	}


	ImGui::Separator();

	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::BeginPopupContextItem("item context menu"))
		{
			if (ImGui::Selectable("Reset"))
			{
				URManager->AddState(cReset::Create(this));
				position = D3DXVECTOR3(0, 0, 0);
				rotate = D3DXVECTOR3(0, 0, 0);
				scale = D3DXVECTOR3(1, 1, 1);
				editBuf.position = position;
				editBuf.rotate = rotate;
				editBuf.scale = scale;
				Rotation(rotate);				
			}
			ImGui::EndPopup();
		}


		if (ImGui::DragFloat3("Position", position))
		{
			Position(position);
		}

		if (ImGui::DragFloat3("Rotate", rotate))
		{
			Rotation(rotate);
		}
		if (ImGui::DragFloat3("Scale", scale))
		{
			Scaling(scale);
		}

		if (ImGui::IsMouseReleased(0))
		{
			if (editBuf.position != position)
			{
				URManager->AddState(cPosition::Create(this, editBuf.position));
			}
			if (editBuf.rotate != rotate)
			{
				URManager->AddState(cRotate::Create(this, editBuf.rotate));
			}
			if (editBuf.scale != scale)
			{
				URManager->AddState(cScale::Create(this, editBuf.scale));
			}
		}

		if (!ImGui::IsMouseDown(0))
		{
			editBuf.position = position;
			editBuf.rotate = rotate;
			editBuf.scale = scale;
		}
	}

	ImGui::Separator();

	ImGui::End();
}

void GameRender::Name(string str)
{
	name = str;
	strcpy_s(editBuf.name, str.c_str());
}

string GameRender::Name()
{
	return name;
}

void GameRender::Parent(GameRender * ptr)
{
	if (ptr)
	{
		D3DXMATRIX inv;
		D3DXMATRIX parentWorld = ptr->world;
		if (ptr->parent)
			parentWorld *= ptr->GetParentWorld();
		D3DXMatrixInverse(&inv, NULL, &parentWorld);
		World(world * inv);
		ptr->Parenting(rotate, scale);
	}

	parent = ptr;
}

GameRender * GameRender::Parent()
{
	return parent;
}

void GameRender::Selectable(GameRender ** r)
{
	ImGuiTreeNodeFlags node_flags = 0;

	if (childs.size() > 0)
	{
		node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		bool node_open = ImGui::TreeNodeEx((void*)(int)num, node_flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			if ((*r))
				(*r)->InitSettings();
			*r = this;
			bParenting = true;
		}
		if (ImGui::IsMouseReleased(0))
		{
			if (ImGui::IsItemHoveredRect() && bParenting)
			{
				if (CheckParenting(*r))
				{
					bParenting = false;
					URManager->AddState(cParenting::Create(*r));
					AddChild(*r);
				}
			}
		}
		if (node_open)
		{
			for (UINT i = 0; i < childs.size(); i++)
				childs[i]->Selectable(r);

			ImGui::TreePop();
		}
	}
	else
	{
		node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(int)num, node_flags, name.c_str());
		if (ImGui::IsItemClicked())
		{
			if ((*r))
				(*r)->InitSettings();
			*r = this;
			bParenting = true;
		}
		if (ImGui::IsMouseReleased(0))
		{
			if (ImGui::IsItemHoveredRect() && bParenting)
			{
				if (CheckParenting(*r))
				{
					bParenting = false;
					URManager->AddState(cParenting::Create(*r));
					AddChild(*r);
				}
			}
		}
	}

}

UINT GameRender::ChildCount()
{
	return childs.size();
}

GameRender * GameRender::Child(UINT index)
{
	return childs[index];
}

void GameRender::AddChild(GameRender * val)
{
	val->Separation(this);
	childs.push_back(val);
}

void GameRender::DeleteChild(GameRender * val)
{
	for (UINT i = 0; i < childs.size(); i++)
	{
		if (childs[i] == val)
		{
			D3DXMATRIX mat = childs[i]->World();
			D3DXMATRIX parentWorld = world;
			if (parent)
				parentWorld *= GetParentWorld();
			Separation(childs[i]->rotate, childs[i]->scale);
			childs[i]->World(mat* parentWorld);
			childs[i]->Parent(NULL);
			childs.erase(childs.begin() + i);
			return;
		}
	}
}

void GameRender::Separation(GameRender * val)
{
	if (parent)
		parent->DeleteChild(this);

	Parent(val);
}

bool GameRender::CheckParenting(GameRender * val)
{
	bool b = true;
	b ^= val == this;

	if (parent)
		b &= parent->CheckParenting(val);

	return b;
}

void GameRender::ClearChilds()
{
	for (GameRender* render : childs)
		SAFE_DELETE(render);

	childs.clear();
}

void GameRender::Parenting(D3DXVECTOR3 & rotate, D3DXVECTOR3& scale)
{
	rotate -= this->rotate;
	
	scale.x /= this->scale.x;
	scale.y /= this->scale.y;
	scale.z /= this->scale.z;

	if (parent)
		parent->Parenting(rotate, scale);
}

void GameRender::Separation(D3DXVECTOR3 & rotate, D3DXVECTOR3 & scale)
{
	rotate += this->rotate;

	scale.x *= this->scale.x;
	scale.y *= this->scale.y;
	scale.z *= this->scale.z;

	if (parent)
		parent->Separation(rotate, scale);
}

void GameRender::InitSettings()
{
}

void GameRender::Save(BinaryWriter * w)
{
	w->UInt((UINT)type);
	w->String(name);

	w->Vector3(rotate);
	w->Vector3(scale);
	w->Matrix(world);

	w->UInt(childs.size());
	for (UINT i = 0; i < childs.size(); i++)
	{
		childs[i]->Save(w);
	}
}

void GameRender::Load(BinaryReader * r)
{
	name = r->String();
	rotate = r->Vector3();
	scale = r->Vector3();
	world = r->Matrix();
	World(world);

	UINT count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		RenderType type = (RenderType)r->UInt();
		GameRender* render = NULL;

		render = CreateObject(type);

		render->Load(r);

		render->SetParent(this);

		childs.push_back(render);
	}
}

D3DXMATRIX GameRender::GetParentWorld()
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	if (parent)
	{
		mat = parent->World();
		mat *= parent->GetParentWorld();
	}

	return mat;
	return D3DXMATRIX();
}

GameRender * GameRender::CreateObject(RenderType type)
{
	GameRender* render = NULL;
	wstring matFile = L"";
	wstring meshFile = L"";
	wstring animFolder = L"";

	switch (type)
	{
	case GameRender::RenderType::Unknown:
		render = new GameRender();
		break;
	case GameRender::RenderType::Player:

		break;
	case GameRender::RenderType::Building:
		break;
	case GameRender::RenderType::Terrain:
		render = new Landscape::Terrain(GameRender::root->values);
		break;
	case GameRender::RenderType::PointLight:
		render = new PointLight();
		break;
	case GameRender::RenderType::SpotLight:
		render = new SpotLight();
		break;
	case GameRender::RenderType::Etc:
		break;
	}
	return render;
}

void GameRender::SetValues(ExecuteValues * val)
{
	values = val;
}
