#include "stdafx.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

ModelMesh::ModelMesh()
{
	worldBuffer = new WorldBuffer();
}

ModelMesh::~ModelMesh()
{
	for (ModelMeshPart* meshPart : meshParts)
		SAFE_DELETE(meshPart);

	SAFE_DELETE(worldBuffer);
}

void ModelMesh::Binding()
{
	for (ModelMeshPart* part : meshParts)
		part->Binding();
}

void ModelMesh::Clone(void ** clone)
{
	ModelMesh* mesh = new ModelMesh();
	mesh->name = name;
	mesh->parentBoneIndex = parentBoneIndex;

	for (ModelMeshPart* part : meshParts)
	{
		ModelMeshPart* temp = NULL;
		part->Clone((void **)&temp);

		temp->parent = mesh;
		mesh->meshParts.push_back(temp);
	}

	*clone = mesh;
}

void ModelMesh::RenderWithoutShader()
{
	worldBuffer->SetVSBuffer(1);

	for (ModelMeshPart* part : meshParts)
		part->RenderWithoutShader();
}

void ModelMesh::Render()
{
	worldBuffer->SetVSBuffer(1);

	for (ModelMeshPart* part : meshParts)
		part->Render();
}
