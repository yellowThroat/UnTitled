#include "stdafx.h"
#include "GameModel.h"
#include "../Bounding/Box.h"

GameModel::GameModel(wstring matFile, wstring meshFile)
	: box(NULL)
{
	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);

	shader = new Shader(Shaders + L"999_BoneAnimation.hlsl", "VS_Bone");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	boneBuffer = new BoneBuffer();
	renderBuffer = new RenderBuffer();

	D3DXMatrixIdentity(&rotateMatrix);
}

GameModel::~GameModel()
{
	SAFE_DELETE(renderBuffer);
	SAFE_DELETE(boneBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(model);

	SAFE_DELETE(box);
}

void GameModel::Update()
{
	CalcPosition();

	D3DXMATRIX transformed = Transformed();
	D3DXMATRIX mat = GetParentWorld();

	model->CopyAbsoluteBoneTo(transformed * mat, boneTransforms);


	if (box)
	{
		D3DXMATRIX W;
		W = World();
		box->SetWorld(W * mat);
		box->Update();
	}

	GameRender::Update();
}

void GameModel::Render()
{
	if (Visible() == false) return;


	boneBuffer->SetBones(&boneTransforms[0], boneTransforms.size());
	boneBuffer->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
	{
		int index = mesh->ParentBoneIndex();
		
		renderBuffer->Data.BoneNumber = index;
		renderBuffer->SetVSBuffer(3);
		mesh->Render();
	}

	if (box && bShowBounding)
		box->Render();

	GameRender::Render();
}

void GameModel::Rotate(D3DXVECTOR2 amount)
{
	amount *= Time::Delta();

	D3DXMATRIX axis;
	D3DXMatrixRotationAxis(&axis, &Right(), amount.y);

	D3DXMATRIX Y;
	D3DXMatrixRotationY(&Y, Math::ToRadian(amount.x));

	rotateMatrix = axis * Y;

	D3DXMATRIX world = World();
	World(rotateMatrix * world);
}
