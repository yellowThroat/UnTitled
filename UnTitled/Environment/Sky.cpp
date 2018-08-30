#include "stdafx.h"
#include "./Environment/Sky.h"
#include "./Environment/SkyPlane.h"

Sky::Sky(ExecuteValues* values)
	: values(values)
{
	model = new Model();
	model->ReadMaterial(Models + L"Meshes/Sphere.material");
	model->ReadMesh(Models + L"Meshes/Sphere.mesh");	

	buffer = new Buffer();
	view = false;

	for (Material* material : model->Materials())
		material->SetShader(Shaders + L"999_sky.hlsl");

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterizer[0]);


		desc.FrontCounterClockwise = true;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc;
		States::GetDepthStencilDesc(&desc);
		States::CreateDepthStencil(&desc, &depthStencilState[0]);

		desc.DepthEnable = false;
		States::CreateDepthStencil(&desc, &depthStencilState[1]);
	}
	{
		D3D11_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

		blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		FAILED(D3D::GetDevice()->CreateBlendState(&blendStateDesc, &alphaEnableBlendingState));

		blendStateDesc.RenderTarget[0].BlendEnable = FALSE;

		FAILED(D3D::GetDevice()->CreateBlendState(&blendStateDesc, &alphaDisableBlendingState));
		
		blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		FAILED(D3D::GetDevice()->CreateBlendState(&blendStateDesc, &alphaBlendState2));

	}

	plane = new SkyPlane();
}

Sky::~Sky()
{
	SAFE_DELETE(model);
	SAFE_DELETE(plane);
	SAFE_DELETE(buffer);

	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);

	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);

}

void Sky::Update()
{
	plane->SetWorld(position);

	D3DXMATRIX mat;
	D3DXMatrixTranslation(&mat, position.x, position.y, position.z);
	model->Mesh(0)->SetWorld(mat);

	plane->Frame();
}

void Sky::Render()
{
	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[1], 1);
	D3D::GetDC()->RSSetState(rasterizer[1]);

	
	buffer->SetPSBuffer(2);

	if (view)
		model->Mesh(0)->Render();

	D3D::GetDC()->RSSetState(rasterizer[0]);
	EnableSecondBlendState();

	plane->Render();

	TurnOffAlphaBlending();

	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[0], 1);

}

void Sky::PostRender()
{
	if (ImGui::CollapsingHeader("Sky Color"))
	{
		ImGui::PushID(0);
		ImGui::Checkbox("view", &view);
		ImGui::PopID();
		ImGui::ColorEdit4("Center", buffer->Data.Center);
		ImGui::ColorEdit4("Apex", buffer->Data.Apex);
		ImGui::SliderFloat("Height", &buffer->Data.Height, 0, 5);
	}

	if (ImGui::CollapsingHeader("Cloud"))
	{
		plane->PostRender();
	}
}

void Sky::TurnOnAlphaBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3D::GetDC()->OMSetBlendState(alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void Sky::TurnOffAlphaBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3D::GetDC()->OMSetBlendState(alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void Sky::EnableSecondBlendState()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3D::GetDC()->OMSetBlendState(alphaBlendState2, blendFactor, 0xffffffff);

}
