#include "stdafx.h"
#include "TestFrustum.h"
#include "../Objects/MeshPlane.h"
#include "../Objects/MeshCube.h"
#include "../Viewer/FixedCamera.h"
#include "../Viewer/Frustum.h"
TestFrustum::TestFrustum(ExecuteValues* values)
	: Execute(values)
	, plane(NULL), cube(NULL), frustum(NULL)
{
	plane = new MeshPlane();
	cube = new MeshCube();

	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 10, 1, 10);
	plane->RootAxis(S);

	for (float z = -50; z < 50; z+=10)
		for (float y = -50; y < 50; y+=10)
			for (float x = -50; x < 50; x+=10)
				positions.push_back(D3DXVECTOR3(x, y, z));

	fixed = new FixedCamera();
	fixed->SetPosition(0, 0, -50);
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);
		perspective = new Perspective(desc.Width, desc.Height);
	}

	//frustum = new Frustum(100, fixed, perspective);
	frustum = new Frustum(50, values->MainCamera, values->Perspective);
	frustum->Build();
}

TestFrustum::~TestFrustum()
{	
	SAFE_DELETE(plane);
	SAFE_DELETE(cube);
}

void TestFrustum::Update()
{
	frustum->Build();
	plane->Update();
}

void TestFrustum::PreRender()
{
}

void TestFrustum::Render()
{
	static int count = 0;
	ImGui::Text("Draw Count : %d / %d", count, positions.size());
	count = 0;
	for (UINT i = 0; i < positions.size(); i++)
	{
		if (frustum->ContainPoint(positions[i]))
		{
			cube->Position(positions[i]);

			cube->Update();
			cube->Render();
			
			count++;
		}
	}
	plane->Render();
}

void TestFrustum::PostRender()
{
	
}
