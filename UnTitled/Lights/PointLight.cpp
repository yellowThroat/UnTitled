#include "stdafx.h"
#include "PointLight.h"
#include "../Utilities/BinaryFile.h"
#include "../Bounding/Sphere.h"

PointLight::Buffer* PointLight::buffer = NULL;

PointLight::PointLight()
{
	if (!buffer)
		buffer = new Buffer();

	index = buffer->Data.Count++;
	name = "Point Light";
	strcpy_s(editBuf.name, name.c_str());
	type = RenderType::PointLight;

	sphere = new Shapes::Sphere(D3DXVECTOR3(0, 0, 0), buffer->Data.Light[index].Range);
}

PointLight::~PointLight()
{
	if (buffer)
	{
		buffer->Data.Count--;
		buffer->SetPSBuffer(10);
		if (!buffer->Data.Count)
		{
			SAFE_DELETE(buffer);
			buffer = NULL;
		}
	}
}

void PointLight::Update()
{
	D3DXMATRIX mat = GetParentWorld();
	D3DXMATRIX W = World();

	D3DXVec3TransformCoord
	(
		&buffer->Data.Light[index].Position,
		&position,
		&mat
	);

	W *= mat;
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

	sphere->SetWorld(W);
	sphere->Update();
	GameRender::Update();
}

void PointLight::Render()
{
	buffer->SetPSBuffer(10);
	sphere->Render();
	GameRender::Render();
}

void PointLight::Save(BinaryWriter * w)
{
	w->UInt((UINT)type);
	w->String(name);

	w->Vector3(rotate);
	w->Vector3(scale);
	w->Matrix(world);

	w->Float(buffer->Data.Light[index].intensity);
	w->Vector3(buffer->Data.Light[index].Color);
	w->Float(buffer->Data.Light[index].Range);

	w->UInt(childs.size());
	for (UINT i = 0; i < childs.size(); i++)
	{
		childs[i]->Save(w);
	}

}

void PointLight::Load(BinaryReader * r)
{
	name = r->String();
	rotate = r->Vector3();
	scale = r->Vector3();
	world = r->Matrix();
	World(world);

	buffer->Data.Light[index].intensity = r->Float();
	buffer->Data.Light[index].Color = r->Vector3();
	buffer->Data.Light[index].Range = r->Float();

	sphere->SetRadius(buffer->Data.Light[index].Range);

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

void PointLight::PostRender()
{
	GameRender::PostRender();

	ImGui::Begin("Inspector");

	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::ColorEdit3("Color", buffer->Data.Light[index].Color);
		ImGui::SliderFloat("Intensity", &buffer->Data.Light[index].intensity, 0.1f, 5.0f);
		if (ImGui::SliderFloat("Range", &buffer->Data.Light[index].Range, 0.1f, 15.0f))
			sphere->SetRadius(buffer->Data.Light[index].Range);
	}

	ImGui::End();
}
