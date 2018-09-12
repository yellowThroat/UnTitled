#include "stdafx.h"
#include "Canvas.h"
#include "../Viewer/Orthographic.h"

Canvas::Canvas(ExecuteValues * val)
	: values(val)
{
	quad = new Model();
	//quad->ReadMaterial()
	D3DXMatrixIdentity(&view);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	projection = new Orthographic(0, desc.Width, 0, desc.Height);
}

Canvas::~Canvas()
{
	SAFE_DELETE(quad);
	SAFE_DELETE(shader);
	SAFE_DELETE(projection);
}

void Canvas::Update()
{
}

void Canvas::Render()
{
	quad->Mesh(0)->Render();
}

void Canvas::Create(float w, float h, float sX, float sY)
{
	D3DXMATRIX S, T;
	width = w;
	height = h;
	startX = sX;
	startY = sY;

	D3DDesc desc;
	D3D::GetDesc(&desc);

	D3DXMatrixScaling(&S, width, height, 1);
	D3DXMatrixTranslation(&T, desc.Width / 2.0f + startX, desc.Height / 2.0f - startY, 0.1f);

	quad->Mesh(0)->SetWorld(S * T);
}
