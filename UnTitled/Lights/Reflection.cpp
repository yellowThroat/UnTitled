#include "stdafx.h"
#include "Reflection.h"
#include "../Viewer/Mirror.h"
#include "../Objects/GameRender.h"
#include "../Objects/GameModel.h"

Reflection::Reflection(ExecuteValues* values)
	: values(values)
{	
	mirror = new Mirror();
	mirror->SetRotationDegree(-90, 0);
	mirror->SetPosition(0, 0, 0);

	target = new RenderTarget();
	target->Create(500, 500);

	buffer = new Buffer();

	shader = new Shader(Shaders + L"999_Reflection.hlsl");

	plane = new GameModel(Models + L"Meshes/plane.material", Models + L"Meshes/plane.mesh");
	
	for (Material* material : plane->GetModel()->Materials())
	{
		material->SetShader(shader);
	}

	{
		D3D11_SAMPLER_DESC desc;
		States::GetSamplerDesc(&desc);
		States::CreateSampler(&desc, &sampler);
	}
}

Reflection::~Reflection()
{
	SAFE_DELETE(plane);
	SAFE_DELETE(mirror);
	SAFE_DELETE(target);
	SAFE_DELETE(buffer);
	SAFE_DELETE(shader);
}

void Reflection::PreRender()
{
	target->Set();

	mirror->Update();

	D3DXMATRIX V, P;
	mirror->GetMatrix(&V);

	values->ViewProjection->SetView(V);
	values->ViewProjection->SetVSBuffer(0);

	for (GameRender* render : objects)
	{
		render->Render();
	}
}

void Reflection::Update()
{
	plane->Update();
}

void Reflection::Render()
{
	D3DXMATRIX view;
	mirror->GetMatrix(&view);

	buffer->Matrix(view);
	buffer->SetVSBuffer(4);

	ID3D11ShaderResourceView* srv = target->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, (ID3D11ShaderResourceView **)&srv);
	D3D::GetDC()->PSSetSamplers(10, 1, &sampler);

	plane->Render();
}

void Reflection::Add(GameRender * object)
{
	objects.push_back(object);
}
