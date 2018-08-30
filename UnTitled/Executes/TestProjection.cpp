#include "stdafx.h"
#include "TestProjection.h"
#include "../Objects/MeshPlane.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshSphere.h"
#include "../Viewer/FixedCamera.h"

TestProjection::TestProjection(ExecuteValues* values)
	: Execute(values)
	, plane(NULL), cube(NULL), sphere(NULL)
{
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 3, 3, 3);
	plane = new MeshPlane();
	plane->RootAxis(S);

	cube = new MeshCube();
	cube->Position(D3DXVECTOR3(-3, 1.5f, -2));
	cube->RootAxis(S);

	sphere = new MeshSphere();
	sphere->Position(D3DXVECTOR3(3, 1.5f, -1));
	sphere->RootAxis(S);

	camera = new FixedCamera();
	camera->SetRotationDegree(41, 49);
	camera->SetPosition(-19, 19, -19);
	
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);
		perspective = new Perspective(desc.Width, desc.Height, Math::PI* 0.25f, 1, 100);
	}

	buffer = new ViewProjectionBuffer();

	for (Material* material : sphere->GetModel()->Materials())
	{
		material->SetShader(Shaders + L"998_Projection.hlsl");
		material->SetSpecularMap(Textures + L"shadowMap.png");
		material->SetDiffuse(D3DXCOLOR(0, 0, 1, 1));
		material->SetAmbient(D3DXCOLOR(0, 0, 0.2f, 1));
	}

	for (Material* material : cube->GetModel()->Materials())
	{
		material->SetShader(Shaders + L"998_Projection.hlsl");
		material->SetSpecularMap(Textures + L"shadowMap.png");
		material->SetDiffuse(D3DXCOLOR(0, 1, 1, 1));
		material->SetAmbient(D3DXCOLOR(0, 0.2f, 0.2f, 1));

	}

	for (Material* material : plane->GetModel()->Materials())
	{
		material->SetShader(Shaders + L"998_Projection.hlsl");
		material->SetSpecularMap(Textures + L"shadowMap.png");
		material->SetDiffuse(D3DXCOLOR(1, 0, 1, 1));
		material->SetAmbient(D3DXCOLOR(0.2f, 0, 0.2f, 1));

	}

}

TestProjection::~TestProjection()
{	
	SAFE_DELETE(plane);
	SAFE_DELETE(cube);
	SAFE_DELETE(sphere);
	SAFE_DELETE(buffer);
}

void TestProjection::Update()
{
	plane->Update();
	cube->Update();
	sphere->Update();

	D3DXMATRIX V, P, T;
	camera->GetMatrix(&V);
	perspective->GetMatrix(&P);

	values->MainCamera->GetMatrix(&T);
	values->Perspective->GetMatrix(&P);

	buffer->SetView(V);
	buffer->SetProjection(P);
}

void TestProjection::PreRender()
{
}

void TestProjection::Render()
{
	buffer->SetVSBuffer(10);
	plane->Render();
	cube->Render();
	sphere->Render();
}

void TestProjection::PostRender()
{
	
}
