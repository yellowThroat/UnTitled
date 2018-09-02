#include "stdafx.h"
#include "ExportMesh.h"
#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues* values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;
	//exporter->ExportMaterial(Models + L"Characters/Player/", L"Eve");
	//exporter->ExportMesh(Models + L"Characters/Player/", L"Eve");

	//exporter = new Fbx::Exporter(Assets + L"Jump.fbx");
	//exporter->ExportAnimation(Models + L"Animation/", L"Jump");

	//vector<wstring> files;
	//Path::GetFiles(&files, Assets, L"*.fbx", false);

	//for (UINT i = 0; i < files.size(); i++)
	//{
	//	wstring folder = Models + L"Animation/";
	//	wstring name = Path::GetFileNameWithoutExtension(files[i]);
	//	exporter = new Fbx::Exporter(files[i]);
	//	exporter->ExportAnimation(folder, name);
	//}

}

ExportMesh::~ExportMesh()
{

}

void ExportMesh::Update()
{
	
}

void ExportMesh::PreRender()
{

}

void ExportMesh::Render()
{
	
}

void ExportMesh::PostRender()
{

}