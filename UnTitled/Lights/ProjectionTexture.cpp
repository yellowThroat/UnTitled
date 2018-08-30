#include "stdafx.h"
#include "ProjectionTexture.h"
#include "../Utilities/BinaryFile.h"
#include "../Viewer/FixedCamera.h"
#include "../Viewer/Perspective.h"


ProjectionTexture::ProjectionTexture()
	: texture(NULL)
{
	buffer = new Buffer();

	camera = new FixedCamera();

	{
		D3DDesc desc;
		D3D::GetDesc(&desc);
		perspective = new Perspective(desc.Width, desc.Height);
	}

	texture = new Texture(Textures + L"shadowMap.png");
}

ProjectionTexture::~ProjectionTexture()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(texture);
	SAFE_DELETE(camera);
	SAFE_DELETE(perspective);
}

void ProjectionTexture::Update()
{
	D3DXMATRIX V, P;
	camera->SetRotationDegree(rotate.x, rotate.y);
	camera->SetPosition(position.x, position.y, position.z);
	camera->GetMatrix(&V);
	perspective->GetMatrix(&P);

	buffer->SetView(V);
	buffer->SetProjection(P);

	GameRender::Update();
}

void ProjectionTexture::Render()
{
	buffer->SetVSBuffer(10);
	texture->SetShaderResource(5);
	texture->SetShaderSampler(5);
	GameRender::Render();
}

void ProjectionTexture::Save(BinaryWriter * w)
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

void ProjectionTexture::Load(BinaryReader * r)
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

void ProjectionTexture::PostRender()
{
	GameRender::PostRender();
}
