#include "stdafx.h"
#include "DirectionalLight.h"
#include "../Viewer/FixedCamera.h"

DirectionalLight::DirectionalLight(ExecuteValues * val)
	:values(val)
{
	light = new FixedCamera();
	shader = new Shader(Shaders + L"998_shadow.hlsl");
	target = new RenderTarget();
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&projection);
}

DirectionalLight::~DirectionalLight()
{
	SAFE_DELETE(light);
	SAFE_DELETE(shader);
	SAFE_DELETE(target);
}

void DirectionalLight::AddObject(GameRender * object)
{
	objects.push_back(object);
}

void DirectionalLight::Update()
{
	D3DXVECTOR3 pos = values->GlobalLight->Data.Direction;
	float x, y;
	Math::RadianFromDirection(x, y, pos);
	light->SetRotation(x, y);
	pos *= -100.0f;
	light->SetPosition(pos.x, pos.y, pos.z);

	light->GetMatrix(&view);

	D3DXMatrixPerspectiveFovLH(&projection, Math::PI* 0.5f, 1.0f, 1.0f, 1000.0f);
}

void DirectionalLight::Render()
{
	target->Set(0xFF000000);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);
}
