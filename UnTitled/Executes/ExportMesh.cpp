#include "stdafx.h"
#include "ExportMesh.h"
#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues* values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;
	//exporter = new Fbx::Exporter(Assets + L"Zombie.fbx");
	//exporter->ExportMesh(Models + L"Characters/Monsters/", L"Zombie");
	//exporter = new Fbx::Exporter(Assets + L"ToExportModel/Zombie.fbx");
	//exporter->ExportMesh(Models + L"Characters/Monsters/", L"Zombie");
	//exporter->ExportMaterial(Models + L"Characters/Monsters/", L"Zombie");
	//exporter = new Fbx::Exporter(Assets + L"Idle.fbx");
	//exporter->ExportAnimation(Models + L"Animation/", L"Idle");
	//exporter->ExportMaterial(Models + L"Characters/Player/", L"Eve");

	//exporter->ExportAnimation(Models + L"Animation/", L"Lead Jap");

	vector<wstring> files;
	Path::GetFiles(&files, Assets + L"ToExportAnimation/", L"*.fbx", false);

	for (UINT i = 0; i < files.size(); i++)
	{
		wstring folder = Models + L"Animation/";
		wstring name = Path::GetFileNameWithoutExtension(files[i]);
		exporter = new Fbx::Exporter(files[i]);
		exporter->ExportAnimation(folder, name);
		SAFE_DELETE(exporter);
	}
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