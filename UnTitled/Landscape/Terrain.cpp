#include "stdafx.h"
#include "Terrain.h"
#include "Data.h"
#include "Ocean.h"

#include "../Utilities/Path.h"
#include "../Utilities/BinaryFile.h"
#include "../Commands/Command.h"

Texture* Landscape::Terrain::brush = NULL;

Landscape::Terrain::Terrain(ExecuteValues *  values)
	: values(values)
	, mode(0), flatteningHeight(0.0f)
	, material(NULL), worldBuffer(NULL), gridBuffer(NULL)
	, data(NULL), brushBuffer(NULL), ocean(NULL)
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	D3DXMATRIX proj, view;
	values->MainCamera->GetMatrix(&view);
	values->Perspective->GetMatrix(&proj);

	oShader = new Shader(Shaders + L"027_Tool.hlsl");
	material = new Material();
	material->SetShader(oShader);
	material->SetDetailMap(Contents + L"Landscape/detail.jpg");

	worldBuffer = new WorldBuffer();
	gridBuffer = new GridBuffer();

	data = new Data();

	brushBuffer = new BrushBuffer();

	size.x = data->GetWidth();
	size.y = data->GetHeight();
	
	name = "Terrain";
	strcpy_s(editBuf.name, name.c_str());
	type = RenderType::Terrain;
	//ocean = new Ocean(values, data->GetWidth(), data->GetHeight(),8);
	isBoneBuffer->Data.isBone = 0;
}

Landscape::Terrain::~Terrain()
{
	SAFE_DELETE(brushBuffer);
	SAFE_DELETE(data);
	SAFE_DELETE(ocean);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(material);
	
	SAFE_DELETE(gridBuffer);
}

float Landscape::Terrain::GetVertexHeight(D3DXVECTOR3 position)
{
	return data->GetHeight(position);
}

void Landscape::Terrain::Update()
{
	InputHandle();

	GameRender::Update();

	if (ocean)
		ocean->Update();
}

void Landscape::Terrain::Render()
{
	D3DXMATRIX mat = World();
	if (parent)
		mat *= GetParentWorld();
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	world._41 = mat._41;
	world._42 = mat._42;
	world._43 = mat._43;

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);
	gridBuffer->SetPSBuffer(2);
	brushBuffer->SetPSBuffer(3);
	material->SetBuffer();
	isBoneBuffer->SetVSBuffer(4);
	data->Render();
	if (ocean)
		ocean->Render();
	GameRender::Render();
}

void Landscape::Terrain::PostRender()
{
	GameRender::PostRender();

	ImGui::Begin("Inspector");	

	if (ImGui::CollapsingHeader("Terrain"))
	{
		if (ImGui::TreeNode("Brush"))
		{
			if (Keyboard::Get()->Down('1'))
				brushBuffer->Info.Type = 1;
			if (Keyboard::Get()->Down('2'))
				brushBuffer->Info.Type = 2;
			if (Keyboard::Get()->Down('3'))
				brushBuffer->Info.Type = 3;
			for (int i = 0; i < 3; i++)
			{
				ImGui::PushID(i);
				if (ImGui::ImageButton(brush->GetView(), { 32,32 }, { 0.333f * i,0 }, { 0.333f + 0.333f*i,1 }))
					brushBuffer->Info.Type = i + 1;
				
				if (ImGui::IsItemHovered())
				{
					string tip = "";
					switch (i)
					{
					case 0:
						tip = "Draw Range '1'";
						break;
					case 1:
						tip = "Circle Brush '2'";
						break;
					case 2:
						tip = "square Brush '3'";
						break;
					}
					ImGui::SetTooltip(tip.c_str());
				}
				ImGui::PopID();
				ImGui::SameLine();
			}
			ImGui::NewLine();
			ImGui::Text("X : %.2f, Y : %.2f, Z : %.2f", brushBuffer->Info.Position.x, brushBuffer->Info.Position.y, brushBuffer->Info.Position.z);
			ImGui::ColorEdit4("Color", brushBuffer->Info.Color);
			ImGui::SliderFloat("Range", &brushBuffer->Info.Range, 0, 30);
			ImGui::SliderFloat("Speed", &brushBuffer->Info.Speed, 0.5f, 5.0f);
			ImGui::RadioButton("Height", &mode, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Paint Height", &mode, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Smooth", &mode, 2);

			switch (mode)
			{
			case 0:
			{
				ImGui::Text(" Raise/Lower Terrain. \n Click to Raise. \n Hold Alt and Click to Lower. \n Hold Shift to fix brush");
				float size_x = ImGui::GetWindowWidth() - 20;
				float size_y = ImGui::GetItemRectMax().y + 10;
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImVec2(size_x, size_y), IM_COL32(204, 166, 61, 255));
			}

			break;
			case 1:
			{
				ImGui::DragFloat("Height ", &flatteningHeight, 0.5f, 0.0f);
				ImGui::Text(" Paint Height. \n Click to Paint Height. \n Hold Shift and click \n to sample target height.");
				float size_x = ImGui::GetWindowWidth() - 20;
				float size_y = ImGui::GetItemRectMax().y + 10;
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImVec2(size_x, size_y), IM_COL32(204, 166, 61, 255));
			}

				break;
			case 2:
			{
				ImGui::Text(" Smooth Height. \n Click to Average out height.");
				float size_x = ImGui::GetWindowWidth() - 20;
				float size_y = ImGui::GetItemRectMax().y + 10;
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImVec2(size_x, size_y), IM_COL32(204, 166, 61, 255));
			}

				break;
			}

			ImGui::NewLine();
			ImGui::TreePop();
		}
		else brushBuffer->Info.Type = 0;



		ImGui::Separator();

		if (ImGui::TreeNode("Grid"))
		{
			gridBuffer->Grid.bView = true;

			ImGui::ColorEdit3("Grid Color", gridBuffer->Grid.Color);
			ImGui::SliderInt("Grid Spacing", (int*)&gridBuffer->Grid.Spacing, 1, 10);
			ImGui::SliderFloat("Grid Thickness", &gridBuffer->Grid.Thickness, 0.01f, 0.1f);
			

			ImGui::TreePop();
		}
		else gridBuffer->Grid.bView = false;


		ImGui::Separator();
	}
	else
	{
		brushBuffer->Info.Type = 0;
		gridBuffer->Grid.bView = false;
	}
	
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Material"))
	{
		if (ImGui::Button("Set DiffuseMap", { 130, 20 }))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);

			function<void(wstring)> f;
			f = bind(&Terrain::SetDiffuse, this, placeholders::_1);

			Path::OpenFileDialog(L"", Path::ImageFilter, LandScape, f, desc.Handle);
		}
		ImGui::SameLine();
		if (ImGui::Button("Set Height", { 130, 20 }))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);

			function<void(wstring)> f;
			f = bind(&Terrain::SetHeight, this, placeholders::_1);

			Path::OpenFileDialog(L"", Path::ImageFilter, LandScape, f, desc.Handle);
		}
		if (material->GetDiffuseMap())
		{
			ImGui::Image(material->GetDiffuseMap()->GetView(), ImVec2(64, 64));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Diffuse Map");
		}
	}

	if (ImGui::CollapsingHeader("Ocean"))
	{
		ocean->PostRender();
	}

	ImGui::End();	
}

void Landscape::Terrain::SetDiffuse(wstring file)
{
	if (file.length() < 1) return;
	URManager->AddState(cTexture::Create(this));
	material->GetBuffer()->Data.haveDiffuse = 1;
	material->SetDiffuseMap(file);
}

void Landscape::Terrain::SetHeight(wstring file)
{
	if (file.length() < 1) return;
	data->SetHeightMapFile(file);	
}

void Landscape::Terrain::ShaderFile(Shader* s)
{
	if (!s)
	{
		material->SetShader(oShader);

		return;
	}

	material->SetShader(s);
}

void Landscape::Terrain::InitSettings()
{
	gridBuffer->Grid.bView = false;
	brushBuffer->Info.Type = 0;
}

void Landscape::Terrain::Save(BinaryWriter * w)
{
	w->UInt((UINT)type);
	w->String(name);

	w->Vector3(rotate);
	w->Vector3(scale);
	w->Matrix(world);
	data->SaveData(w);

	if (material->GetDiffuseMap())
	{
		w->Bool(true);
		wstring name = material->GetDiffuseMap()->GetFile();
		w->String(String::ToString(name));
	}
	else w->Bool(false);
	
	

	w->UInt(childs.size());
	for (UINT i = 0; i < childs.size(); i++)
	{
		childs[i]->Save(w);
	}
}

void Landscape::Terrain::Load(BinaryReader * r)
{
	name = r->String();
	rotate = r->Vector3();
	scale = r->Vector3();
	world = r->Matrix();
	World(world);
	data->LoadData(r);
	bool b = r->Bool();
	if (b)
	{
		string name = r->String();
		SetDiffuse(String::ToWString(name));
	}
	

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

void Landscape::Terrain::InputHandle()
{
	bool picking = false;
	D3DXMATRIX W = world;
	if (parent)
		W *= GetParentWorld();

	if (brushBuffer->Info.Type > 1)
	{
		if (!mode && Keyboard::Get()->Press(VK_SHIFT))
		{
			if (!Keyboard::Get()->Press(VK_LBUTTON))
				picking = data->GetPosition(values, W, &brushBuffer->Info.Position);
			else picking = true;
		}
		else
			picking = data->GetPosition(values, W, &brushBuffer->Info.Position);

	}
	
	if (Keyboard::Get()->Down(VK_LBUTTON))
	{
		if (brushBuffer->Info.Type == 1)
		{
			brushBuffer->Info.Select = (int)data->GetPosition(values, W, &brushBuffer->Info.Position);

			if (brushBuffer->Info.Select)
			{
				brushBuffer->Info.Before = brushBuffer->Info.Position;
			}
		}
		else
		{
			brushBuffer->Info.Select = 0;
		}

		if (brushBuffer->Info.Type > 1)
		{
			if (picking)
			{
				if (!Keyboard::Get()->Press(VK_SHIFT) && mode != 1)
					URManager->AddState(cData::Create(this));
			}
		}
	}
	
	if (Keyboard::Get()->Down(VK_UP))
	{
		if (brushBuffer->Info.Type == 1 && brushBuffer->Info.Select)
		{
			URManager->AddState(cData::Create(this));
		}
	}

	if (Keyboard::Get()->Down(VK_DOWN))
	{
		if (brushBuffer->Info.Type == 1 && brushBuffer->Info.Select)
		{
			URManager->AddState(cData::Create(this));
		}
	}
	
	if (Keyboard::Get()->Press(VK_LBUTTON))
	{
		
		if (picking && brushBuffer->Info.Type > 1)
		{
			if (Keyboard::Get()->Press(VK_MENU))
			{
				if (!mode)
					data->SetHeight(*gridBuffer, *brushBuffer, false);
			}
			else
			{
				if (!mode)
				{
					data->SetHeight(*gridBuffer, *brushBuffer, true);
				}
				else if (mode == 1)
				{
					if (!Keyboard::Get()->Press(VK_SHIFT))
						data->Flattening(*gridBuffer, *brushBuffer, flatteningHeight);
					else
					{
						D3DXVECTOR3 temp;
						data->GetPosition(values, W, &temp);
						flatteningHeight = temp.y;
					}

				}
				else if (mode == 2)
				{
					data->Smooth(*gridBuffer, *brushBuffer);
				}
			}
		}
		
		if (brushBuffer->Info.Select)
		{
			data->GetPosition(values, W, &brushBuffer->Info.Position);
		}
	}
	
	if (Keyboard::Get()->Press(VK_UP))
	{
		if (brushBuffer->Info.Type == 1)
			data->SetHeight(*gridBuffer, *brushBuffer, true);
	}

	if (Keyboard::Get()->Press(VK_DOWN))
	{
		if (brushBuffer->Info.Type == 1)
			data->SetHeight(*gridBuffer, *brushBuffer, false);
	}

	if (Keyboard::Get()->Up(VK_LBUTTON))
	{
		if (brushBuffer->Info.Type > 1)
		{
			data->FillNormalData();
			data->UpdateVertexData();
		}
	}
	if (Keyboard::Get()->Up(VK_UP))
	{
		if (brushBuffer->Info.Type == 1)
		{
			data->FillNormalData();
			data->UpdateVertexData();
		}
	}
	if (Keyboard::Get()->Up(VK_DOWN))
	{
		if (brushBuffer->Info.Type == 1)
		{
			data->FillNormalData();
			data->UpdateVertexData();
		}
	}


}