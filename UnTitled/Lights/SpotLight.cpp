#include "stdafx.h"
#include "SpotLight.h"
#include "../Utilities/BinaryFile.h"
#include "../Bounding/Cone.h"

SpotLight::Buffer* SpotLight::buffer = NULL;

SpotLight::SpotLight()
{
	if (!buffer)
		buffer = new Buffer();

	index = buffer->Data.Count++;
	name = "Spot Light";
	strcpy_s(editBuf.name, name.c_str());
	type = RenderType::SpotLight;
	cone = new Shapes::Cone
	(
		Math::ToRadian(buffer->Data.Light[index].OuterAngle) / 2.0f,
		buffer->Data.Light[index].Range
	);
}

SpotLight::~SpotLight()
{
	if (buffer)
	{
		buffer->Data.Count--;
		buffer->SetPSBuffer(11);
		if (!buffer->Data.Count)
		{
			SAFE_DELETE(buffer);
			buffer = NULL;
		}
	}
}

void SpotLight::Update()
{
	D3DXMATRIX mat = GetParentWorld();
	D3DXMATRIX W = World() * mat;

	D3DXVec3TransformCoord
	(
		&buffer->Data.Light[index].Position,
		&position,
		&mat
	);

	D3DXVec3TransformNormal
	(
		&buffer->Data.Light[index].Direction,
		&D3DXVECTOR3(0, 0, 1),
		&W
	);

	D3DXVECTOR3 r, u, d;
	r = D3DXVECTOR3(W._11, W._12, W._13);
	u = D3DXVECTOR3(W._21, W._22, W._23);
	d = D3DXVECTOR3(W._31, W._32, W._33);
	D3DXVec3Normalize(&r, &r);
	D3DXVec3Normalize(&u, &u);
	D3DXVec3Normalize(&d, &d);
	W._11 = r.x;
	W._12 = r.y;
	W._13 = r.z;
	W._21 = u.x;
	W._22 = u.y;
	W._23 = u.z;
	W._31 = d.x;
	W._32 = d.y;
	W._33 = d.z;


	cone->SetWorld(W);
	cone->Update();
	GameRender::Update();
}

void SpotLight::Render()
{
	buffer->SetPSBuffer(11);
	cone->Render();
	GameRender::Render();
}

void SpotLight::Save(BinaryWriter * w)
{
	w->UInt((UINT)type);
	w->String(name);

	w->Vector3(rotate);
	w->Vector3(scale);
	w->Matrix(world);

	w->Float(buffer->Data.Light[index].InnerAngle);
	w->Vector3(buffer->Data.Light[index].Color);
	w->Float(buffer->Data.Light[index].OuterAngle);

	w->UInt(childs.size());
	for (UINT i = 0; i < childs.size(); i++)
	{
		childs[i]->Save(w);
	}

}

void SpotLight::Load(BinaryReader * r)
{
	name = r->String();
	rotate = r->Vector3();
	scale = r->Vector3();
	world = r->Matrix();
	World(world);

	buffer->Data.Light[index].InnerAngle = r->Float();
	buffer->Data.Light[index].Color = r->Vector3();
	buffer->Data.Light[index].OuterAngle = r->Float();

	cone->SetAngle(Math::ToRadian(buffer->Data.Light[index].OuterAngle) / 2.0f);
	cone->SetHeight(buffer->Data.Light[index].Range);

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

void SpotLight::PostRender()
{
	GameRender::PostRender();

	ImGui::Begin("Inspector");
	
	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::ColorEdit3("Color", buffer->Data.Light[index].Color);
		ImGui::SliderFloat("Inner Angle", &buffer->Data.Light[index].InnerAngle, 1, 179);
		if (ImGui::SliderFloat("Outer Angle", &buffer->Data.Light[index].OuterAngle, 1, 179))
			cone->SetAngle
			(
				Math::ToRadian(buffer->Data.Light[index].OuterAngle) / 2.0f
			);
		if (ImGui::SliderFloat("Range", &buffer->Data.Light[index].Range, 1.0f, 30.0f))
			cone->SetHeight
			(
				buffer->Data.Light[index].Range
			);
	}

	ImGui::End();
}
