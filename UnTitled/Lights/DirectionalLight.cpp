#include "stdafx.h"
#include "DirectionalLight.h"
#include "../Objects/GameRender.h"
#include "../Viewer/FixedCamera.h"

DirectionalLight::DirectionalLight(ExecuteValues * val)
	:values(val)
	, position(NULL)
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
	pos *= -20.0f;
	if (position)
		pos += *position;
	light->SetPosition(pos.x, pos.y, pos.z);

	light->GetMatrix(&view);

	D3DXMatrixPerspectiveFovLH(&projection, Math::PI* 0.5f, 1.0f, 1.0f, 1000.0f);
}

void DirectionalLight::Render()
{
	target->Set(0xFFFFFFFF);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);


	for (auto object : objects)
	{
		object->ShaderFile(shader);
		object->PreRender();
	}

	if (Keyboard::Get()->Down('C'))
	{
		//ID3D11Texture2D* t = renderTarget->GetTexture()
		Texture::SaveFile(L"test.png", target->GetTexture());
	}

}

void DirectionalLight::EraseObject(UINT num)
{
	for (UINT i = 0; i < objects.size();)
	{
		if (objects[i]->GetNum() == num)
		{
			objects.erase(objects.begin() + i);
			break;
		}
		else i++;
	}
}

void DirectionalLight::Clear()
{
	objects.clear();
}
