#include "stdafx.h"
#include "ModelAnimPlayer.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelAnimClip.h"

ModelAnimPlayer::ModelAnimPlayer(Model * model)
	: model(model)
	, mode(Mode::Play)
	, currentClip(NULL), nextClip(NULL)
	, BindCount(0), blendTime(0.0f)
{
	shader = new Shader(Shaders + L"999_Animation.hlsl");
	
	vector<Material *>& materials = model->Materials();
	for (Material* material : materials)
		material->SetShader(shader);

	currentClip = new Binder();
	nextClip = new Binder();

	Play(0, 0.0f, true);

	for (UINT i = 0; i < model->ClipCount(); i++)
	{
		clipList += String::ToString(model->Clip(i)->Name());
		clipList.push_back('\0');
	}

	D3DXMATRIX M;
	D3DXMatrixIdentity(&M);
	D3DXMatrixIdentity(&rootAxis);
	skinTransform.assign(model->BoneCount(), M);
	boneAnimation.assign(model->BoneCount(), M);
}

ModelAnimPlayer::~ModelAnimPlayer()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(currentClip);
	SAFE_DELETE(nextClip);
}

void ModelAnimPlayer::Update()
{
	if (currentClip == NULL)
		return;

	if(mode == Mode::Play)
	UpdateTime();
	UpdateBone();
	model->Buffer()->SetBones(&skinTransform[0], skinTransform.size());
}

void ModelAnimPlayer::Render()
{
	ImGui::Begin("Animation Player");

	ImGui::Text(String::ToString(currentClip->clip->Name()).c_str());
	if (ImGui::Button("Play"))	Play(); ImGui::SameLine();
	if (ImGui::Button("Stop"))	Stop(); ImGui::SameLine();
	if (ImGui::Button("Pause"))	Pause();

	ImGui::End();

	model->Buffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

void ModelAnimPlayer::World(D3DXMATRIX * world)
{
	this->world = world;
}

void ModelAnimPlayer::RootAxis(D3DXMATRIX root)
{
	rootAxis = root;
}

D3DXMATRIX ModelAnimPlayer::GetTransform(UINT index)
{
	return skinTransform[index];
}

void ModelAnimPlayer::Play(UINT index, float blendTime, bool bLoop)
{
	this->blendTime = blendTime;
	if (blendTime == 0.0f)
	{
		currentClip->Initialize();
		nextClip->Initialize();
		*currentClip = model->Clip(index);
		currentClip->bLoop = bLoop;
		BindCount = 1;
	}
	else
	{
		if (BindCount == 2)
		{
			currentClip->CopyBinder(nextClip);
			nextClip->Initialize();
			BindCount--;
		}

		if (BindCount == 1)
		{
			nextClip->Initialize();
			*nextClip = model->Clip(index);
			nextClip->bLoop = bLoop;
			BindCount++;
		}
		else
		{
			currentClip->Initialize();
			*currentClip = model->Clip(index);
			currentClip->bLoop = bLoop;
			BindCount++;
		}
	}
}


void ModelAnimPlayer::UpdateTime()
{
	if (BindCount <= 1)
		currentClip->UpdateTime();
	else
	{
		float t = nextClip->elapsedTime / blendTime;

		if (t >= 1.0f)
		{
			currentClip->CopyBinder(nextClip);
			nextClip->Initialize();
			currentClip->UpdateTime();
			BindCount--;
		}
		else
		{
			currentClip->UpdateTime();
			nextClip->UpdateTime();
		}

	}
}

void ModelAnimPlayer::UpdateBone()
{

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		D3DXMATRIX matAnimation;
		D3DXMATRIX matParentAnimation;
		D3DXMATRIX S, R, T;
		D3DXVECTOR3 vS, vT;
		D3DXQUATERNION Q;

		D3DXMatrixIdentity(&matAnimation);

		D3DXMATRIX matInvBindPose = bone->AbsoluteTransform();
		D3DXMatrixInverse(&matInvBindPose, NULL, &matInvBindPose);

		if (BindCount <= 1)
		{
			matAnimation = currentClip->GetCurrentTransform(bone->Name(), vS, Q, vT);
		}
		else
		{
			float t = nextClip->elapsedTime / blendTime;
			D3DXVECTOR3 s0, s1, t0, t1;
			D3DXQUATERNION q0, q1;
			currentClip->GetCurrentTransform(bone->Name(), s0, q0, t0);
			nextClip->GetCurrentTransform(bone->Name(), s1, q1, t1);

			D3DXVec3Lerp(&vS, &s0, &s1, t);
			D3DXVec3Lerp(&vT, &t0, &t1, t);
			D3DXQuaternionSlerp(&Q, &q0, &q1, t);
			
			D3DXMatrixScaling(&S, vS.x, vS.y, vS.z);
			D3DXMatrixRotationQuaternion(&R, &Q);
			D3DXMatrixTranslation(&T, vT.x, vT.y, vT.z);

			matAnimation = S* R* T;
		}

		int parentIndex = bone->ParentIndex();
		
		if (parentIndex < 0)
			matParentAnimation = rootAxis * (*world);
		else
			matParentAnimation = boneAnimation[parentIndex];

		boneAnimation[i] = matAnimation * matParentAnimation;
		skinTransform[i] = matInvBindPose * boneAnimation[i];
	}
}

void ModelAnimPlayer::Stop()
{
	mode = Mode::Stop;
}

void ModelAnimPlayer::Play()
{
	mode = Mode::Play;
}

void ModelAnimPlayer::Pause()
{
	mode = Mode::Pause;
}

void Binder::UpdateTime()
{
	frameTime += Time::Delta();
	elapsedTime += Time::Delta();

	float invFrameRate = 1.0f / clip->FrameRate();

	while (frameTime > invFrameRate)
	{
		int keyframeCount = clip->TotalFrame();

		currentKeyframe++;
		nextKeyframe = currentKeyframe + 1;

		if (bLoop)
		{
			currentKeyframe %= keyframeCount;
			nextKeyframe %= keyframeCount;
		}
		else if(currentKeyframe >= keyframeCount - 1)
		{
			currentKeyframe = keyframeCount - 1;
			nextKeyframe = 0;
		}
		frameTime -= invFrameRate;
	}

	keyframeFactor = frameTime / invFrameRate;

}

void Binder::CopyBinder(Binder * binder)
{
	clip = binder->clip;
	currentKeyframe = binder->currentKeyframe;
	frameTime = binder->frameTime;
	keyframeFactor = binder->keyframeFactor;
	nextKeyframe = binder->nextKeyframe;
	elapsedTime = binder->elapsedTime;
	bLoop = binder->bLoop;
}

bool Binder::IsDone()
{
	if (!bLoop && currentKeyframe >= clip->TotalFrame() - 1)
		return true;

	return false;
}

D3DXMATRIX Binder::GetCurrentTransform(wstring name, D3DXVECTOR3 & vS, D3DXQUATERNION & vQ, D3DXVECTOR3 & vT)
{
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	ModelKeyframe* frame = clip->Keyframe(name);

	if (!frame)
	{
		vS = D3DXVECTOR3(1, 1, 1);
		vQ = D3DXQUATERNION(0, 0, 0, 0);
		vT = D3DXVECTOR3(0, 0, 0);
		return identity;
	}
	D3DXMATRIX S, R, T;

	ModelKeyframeData current = frame->Datas[currentKeyframe];
	ModelKeyframeData next = frame->Datas[nextKeyframe];

	D3DXVECTOR3 s1 = current.Scale;
	D3DXVECTOR3 s2 = next.Scale;

	D3DXVECTOR3 s;
	D3DXVec3Lerp(&s, &s1, &s2, keyframeFactor);
	D3DXMatrixScaling(&S, s.x, s.y, s.z);


	D3DXQUATERNION q1 = current.Rotation;
	D3DXQUATERNION q2 = next.Rotation;

	D3DXQUATERNION q;
	D3DXQuaternionSlerp(&q, &q1, &q2, keyframeFactor);
	D3DXMatrixRotationQuaternion(&R, &q);

	D3DXVECTOR3 t1 = current.Translation;
	D3DXVECTOR3 t2 = next.Translation;

	D3DXVECTOR3 t;
	D3DXVec3Lerp(&t, &t1, &t2, keyframeFactor);
	D3DXMatrixTranslation(&T, t.x, t.y, t.z);

	vS = s;
	vQ = q;
	vT = t;

	return S*R*T;
}
