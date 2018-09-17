#include "stdafx.h"

#include "DrawGui.h"
#include "../Viewer/Orthographic.h"

DrawGui::DrawGui(ExecuteValues * values)
	: Execute(values)
{
	shader = new Shader(Shaders + L"Canvas.hlsl");
	D3DXMatrixIdentity(&view);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	projection = new Orthographic(0, desc.Width, 0, desc.Height);
	D3D11_BLEND_DESC alpha;
	States::GetBlendDesc(&alpha);
	States::CreateBlend(&alpha, &blendState[0]);

	alpha.RenderTarget[0].BlendEnable = true;
	States::CreateBlend(&alpha, &blendState[1]);

}

DrawGui::~DrawGui()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(projection);

}

void DrawGui::Update()
{
}

void DrawGui::PreRender()
{
}

void DrawGui::Render()
{
	values->ViewProjection->SetView(view);

	D3DXMATRIX matrix;
	projection->GetMatrix(&matrix);
	values->ViewProjection->SetProjection(matrix);
	values->ViewProjection->SetVSBuffer(0);

	D3D::GetDC()->OMSetBlendState(blendState[1], NULL, 0xFF);



	D3D::GetDC()->OMSetBlendState(blendState[0], NULL, 0xFF);
}

void DrawGui::PostRender()
{
}
