#pragma once
#include "GameRender.h"

namespace Shapes {
	class Box;
}
class GameModel : public GameRender
{
public:
	GameModel(wstring matFile, wstring meshFile);
	virtual ~GameModel();

	virtual void Update();
	virtual void Render();

	void Rotate(D3DXVECTOR2 amount);
	Model* GetModel() { return model; }

protected:
	Shader* shader;
	Model* model;

	D3DXMATRIX rotateMatrix;

	vector<D3DXMATRIX> boneTransforms;
	Shapes::Box* box;
	
private:
	class BoneBuffer : public ShaderBuffer
	{
	public:
		BoneBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++)
				D3DXMatrixIdentity(&Data.Bones[i]);
		}

		void SetBones(D3DXMATRIX* m, UINT count)
		{
			memcpy(Data.Bones, m, sizeof(D3DXMATRIX) * count);

			for (UINT i = 0; i < count; i++)
				D3DXMatrixTranspose(&Data.Bones[i], &Data.Bones[i]);
		}

	private:
		struct Struct
		{
			D3DXMATRIX Bones[128];
		} Data;
	};

	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.BoneNumber = 0;
		}

	public:
		struct Struct
		{
			int BoneNumber;

			float Padding[3];
		} Data;
	};

protected:
	BoneBuffer* boneBuffer;
	RenderBuffer* renderBuffer;
};