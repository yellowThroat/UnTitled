#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Free.h"

#include <fstream>
#include "./Utilities/String.h"

#include "./Executes/ExportMesh.h"
#include "./Executes/ExportAnimation.h"
#include "./Executes/DrawModel.h"
#include "./Executes/DrawEnvironment.h"
#include "./Executes/ExeGui.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->GuiSettings = new GuiSettings();
	values->GlobalLight = new LightBuffer();
	values->GlobalTime = new TimeBuffer();

	values->MainCamera = new Free();
	values->MainCamera->SetPosition(0, 5, -15);
	values->MainCamera->SetRotationDegree(0, 0);

	values->jsonRoot = new Json::Value();
	Json::ReadData(L"LevelEditor.json", values->jsonRoot);

	executes.push_back(new ExportMesh(values));
	executes.push_back(new DrawEnvironment(values));
	executes.push_back(new DrawModel(values));
	executes.push_back(new ExeGui(values));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	Json::WriteData(L"LevelEditor.json", values->jsonRoot);

	SAFE_DELETE(values->jsonRoot);
	SAFE_DELETE(values->MainCamera);
	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values->GuiSettings);
	SAFE_DELETE(values);

	States::Delete();
}

void Program::Update()
{
	values->MainCamera->Update();
	
	
	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0);
	float dot = D3DXVec3Dot(&values->GlobalLight->Data.Direction, &up);
	float intensity = Math::Clamp(dot * 1.5f, 0, 1);
	values->GlobalLight->Data.Intensity = intensity;

	values->GlobalTime->Data.ElapsedTime = Time::Delta();
	values->GlobalTime->Data.RunningTime = Time::Get()->Running();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	D3D::Get()->SetRenderTarget();

	SetGlobalBuffers();

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}

void Program::SetGlobalBuffers()
{
	D3DXMATRIX view, projection;
	values->MainCamera->GetMatrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);
	
	values->GlobalLight->SetPSBuffer(0);
	values->GlobalTime->SetVSBuffer(5);
}
