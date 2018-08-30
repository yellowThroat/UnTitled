#include "stdafx.h"
#include "Model.h"
#include "ModelMesh.h"
#include "ModelBone.h"
#include "ModelAnimClip.h"

Model::Model()
{
	buffer = new ModelBuffer();
}

Model::~Model()
{
	SAFE_DELETE(buffer);

	for (Material* material : materials)
		SAFE_DELETE(material);

	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);

	for (ModelAnimClip* clip : clips)
		SAFE_DELETE(clip);
}

Material * Model::GetMaterial(wstring name)
{
	for (Material* material : materials)
	{
		if (material->GetName() == name)
			return material;
	}

	return NULL;
}

ModelMesh * Model::Mesh(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->name == name)
			return mesh;
	}

	return NULL;
}

ModelBone * Model::Bone(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->name == name)
			return bone;
	}

	return NULL;
}

void Model::CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
		else
			transforms[i] = bone->transform;
	}
}

void Model::CopyAbsoluteBoneTo(D3DXMATRIX matrix, vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
		else
			transforms[i] = bone->transform * matrix;
	}
}

void Model::Render()
{
	vector<D3DXMATRIX> transforms;
	CopyAbsoluteBoneTo(transforms);

	for (ModelMesh* mesh : meshes)
	{
		int index = mesh->ParentBoneIndex();
		D3DXMATRIX transform = transforms[index];

		mesh->SetWorld(transform);
		mesh->Render();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Models::Create()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material *>> temp : materialMap)
	{
		for (Material* material : temp.second)
			SAFE_DELETE(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : data.Meshes)
			SAFE_DELETE(mesh);
	}
}
