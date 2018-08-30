#include "stdafx.h"
#include "ExportMesh.h"
#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues* values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;
	
	//Grund
	//exporter = new Fbx::Exporter(Robots + L"Models/Mech/Player/grund.fbx", true);
	//exporter->ExportMaterial(Robots + L"Models/Mech/Player/", L"Grund");
	//exporter->ExportMesh(Robots + L"Models/Mech/Player/", L"Grund");


	//////World
	//exporter = new Fbx::Exporter(Robots + L"Models/World/Stage1/France.fbx");
	//exporter->ExportMaterial(Robots + L"Models/World/Stage1/", L"France");
	//exporter->ExportMesh(Robots + L"Models/World/Stage1/", L"France");


	////Sky
	//exporter = new Fbx::Exporter(Robots + L"Map/Stage1/France_Sky.fbx");
	//exporter->ExportMaterial(Robots + L"Stage1/", L"France_Sky");
	//exporter->ExportMesh(Robots + L"Stage1/", L"France_Sky");

	//// Tiger
	//exporter = new Fbx::Exporter(Robots + L"Models/Mech/Enemies/Tiger.fbx");
	//exporter->ExportMaterial(Robots + L"Models/Mech/Enemies/", L"Tiger");
	//exporter->ExportMesh(Robots + L"Models/Mech/Enemies/", L"Tiger");

	//// Bullet
	//exporter = new Fbx::Exporter(Assets + L"Tank Shell/Ammo120mm Rheinmetall.fbx");
	//exporter->ExportMaterial(Models + L"Bullets/", L"TankShell");
	//exporter->ExportMesh(Models + L"Bullets/", L"TankShell");

	//// Building
	//exporter = new Fbx::Exporter(Assets + L"farm/farmhouse_fbx.fbx");
	//exporter->ExportMaterial(Models + L"Architecture/", L"farm");
	//exporter->ExportMesh(Models + L"Architecture/", L"farm");

	// bird
	//exporter = new Fbx::Exporter(Assets + L"bird/eagle.fbx");
	//exporter->ExportMaterial(Models + L"bird/", L"eagle");
	//exporter->ExportMesh(Models + L"bird/", L"eagle");
	//exporter->ExportAnimation(Models + L"bird/",L"eagle");

	// Meshes
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Capsule.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Capsule");
	//exporter->ExportMesh(Models + L"Meshes/", L"Capsule");
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Cube");
	//exporter->ExportMesh(Models + L"Meshes/", L"Cube");
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cylinder.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Cylinder");
	//exporter->ExportMesh(Models + L"Meshes/", L"Cylinder");
	//exporter = new Fbx::Exporter(Assets + L"Meshes/plane.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"plane");
	//exporter->ExportMesh(Models + L"Meshes/", L"plane");
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Quad.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Quad");
	//exporter->ExportMesh(Models + L"Meshes/", L"Quad");
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Sphere");
	//exporter->ExportMesh(Models + L"Meshes/", L"Sphere");
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Teapot.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Teapot");
	//exporter->ExportMesh(Models + L"Meshes/", L"Teapot");

	//exporter = new Fbx::Exporter(Assets + L"Styx/Player/test.fbx");
	//exporter->ExportMaterial(Models + L"Styx/Player/", L"test");
	//exporter->ExportMesh(Models + L"Styx/Player/", L"test");
	//exporter->ExportAnimation(Models + L"Styx/Player/", L"test");
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