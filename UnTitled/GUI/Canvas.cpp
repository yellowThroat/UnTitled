#include "stdafx.h"
#include "Canvas.h"
#include "../Viewer/Orthographic.h"

Canvas::Canvas()
{
	quad = new Model();
	quad->ReadMaterial(Models + L"Meshes/Quad.material");
	quad->ReadMesh(Models + L"Meshes/quad.mesh");
}

Canvas::~Canvas()
{
	SAFE_DELETE(quad);
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

	D3DXMatrixScaling(&S, -width, height, 1);
	D3DXMatrixTranslation(&T, width/ 2.0f + startX, desc.Height - height / 2.0f - startY, 0.1f);

	quad->Mesh(0)->SetWorld(S * T);
}

void Canvas::SetTexture(wstring file)
{
	quad->Materials()[0]->SetDiffuseMap(file);
}
