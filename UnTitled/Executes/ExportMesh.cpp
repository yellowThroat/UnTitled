#include "stdafx.h"
#include "ExportMesh.h"
#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues* values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	//exporter = new Fbx::Exporter(Assets + L"Characters/Eve.fbx");
	//exporter->ExportMaterial(Models + L"Characters/Player/", L"Eve");
	//exporter->ExportMesh(Models + L"Characters/Player/", L"Eve");
	//exporter->ExportAnimation(Models + L"Characters/Player/", L"Eve");
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