#pragma once

class ModelBuffer : public ShaderBuffer
{
public:
	ModelBuffer() : ShaderBuffer(&data, sizeof(data))
	{
		for (int i = 0; i < 128; i++)
			D3DXMatrixIdentity(&data.Bones[i]);
	}

	void SetBones(D3DXMATRIX* m, UINT count)
	{
		memcpy(data.Bones, m, sizeof(D3DXMATRIX) * count);

		for (UINT i = 0; i < count; i++)
			D3DXMatrixTranspose(&data.Bones[i], &data.Bones[i]);
	}

private:
	struct Struct
	{
		D3DXMATRIX Bones[128];
	} data;
};

class Model
{
public:
	Model();
	~Model();

	vector<class ModelMesh *>& Meshes() 
	{ 
		return meshes; 
	}

	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadAnimation(wstring file);

	ModelBuffer* buffer;
	ModelBuffer* Buffer() { return buffer; }

	vector<Material *>& Materials() { return materials; }

	Material* GetMaterial(wstring name);

	UINT MeshCount() { return meshes.size(); }
	class ModelMesh* Mesh(UINT index) { return meshes[index]; }
	class ModelMesh* Mesh(wstring name);

	UINT BoneCount() { return bones.size(); }
	class ModelBone* Bone(UINT index) { return bones[index]; }
	class ModelBone* Bone(wstring name);

	class ModelBone* Root() { return root; }

	class ModelAnimClip* Clip(UINT index) { return clips[index]; }

	void CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms);
	void CopyAbsoluteBoneTo(D3DXMATRIX matrix, vector<D3DXMATRIX>& transforms);

	void Render();

private:
	void BindMeshData();

private:
	class ModelBone* root;

	vector<Material *> materials;

	vector<class ModelBone *> bones;
	vector<class ModelMesh *> meshes;
	vector<class ModelAnimClip *> clips;
};

class Models
{
public:
	friend class Model;

public:
	static void Create();
	static void Delete();

private:
	static void LoadMaterial(wstring file, vector<Material *>* materials);
	static void ReadMaterialData(wstring file);
	
	static void LoadMesh(wstring file, vector<class ModelBone *>* bones, vector<class ModelMesh *>* meshes);
	static void ReadMeshData(wstring file);

	static void LoadAnimation(wstring file, vector<class ModelAnimClip *>* clips);
	static void ReadAnimationData(wstring file);

private:
	static map<wstring, vector<Material *>> materialMap;

	struct MeshData
	{
		vector<class ModelBone *> Bones;
		vector<class ModelMesh *> Meshes;
	};
	static map<wstring, MeshData> meshDataMap;

	static map<wstring, vector<class ModelAnimClip *>> animClipMap;
};