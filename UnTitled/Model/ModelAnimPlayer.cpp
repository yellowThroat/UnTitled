#include "stdafx.h"
#include "ModelAnimPlayer.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelAnimClip.h"

ModelAnimPlayer::ModelAnimPlayer(Model * model)
	: model(model)
	, mode(Mode::Play)
	, currentKeyframe(0), frameTime(0.0f), keyframeFactor(0.0)
	, nextKeyframe(0)
	, bUseSlerp(false)
{
	shader = new Shader(Shaders + L"999_Animation.hlsl");
	
	vector<Material *>& materials = model->Materials();
	for (Material* material : materials)
		material->SetShader(shader);

	
	currentClip = model->Clip(0);
	D3DXMATRIX M;
	D3DXMatrixIdentity(&M);
	D3DXMatrixIdentity(&mat);
	skinTransform.assign(model->BoneCount(), M);
	boneAnimation.assign(model->BoneCount(), M);

	r = D3DXVECTOR3(0, 0, 0);
}

ModelAnimPlayer::~ModelAnimPlayer()
{
	SAFE_DELETE(shader);
}

void ModelAnimPlayer::Update()
{
	if (currentClip == NULL)
		return;

	D3DXMatrixRotationYawPitchRoll(&mat, r.y, r.x, r.z);

	if(mode == Mode::Play)
	UpdateTime();
	UpdateBone();
	model->Buffer()->SetBones(&skinTransform[0], skinTransform.size());
}

void ModelAnimPlayer::Render()
{
	ImGui::Separator();

	ImGui::DragFloat3("R", r);
	ImGui::Begin("Animation Player");

	if (ImGui::Button("Play"))
		mode = Mode::Play;
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		mode = Mode::Stop;
		currentKeyframe = 0;
		nextKeyframe = 1;
		frameTime = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
		mode = Mode::Pause;


	if (currentClip)
	{
		ImGui::SliderInt("Currentframe", &currentKeyframe, 0, currentClip->TotalFrame() - 1);
	}


	ImGui::End();

	model->Buffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

void ModelAnimPlayer::UpdateTime()
{
	frameTime += Time::Delta();

	float invFrameRate = 1.0f / currentClip->FrameRate();
	while (frameTime > invFrameRate)
	{
		int keyframeCount = currentClip->TotalFrame();

		currentKeyframe = (currentKeyframe + 1) % keyframeCount;
		nextKeyframe = (currentKeyframe + 1) % keyframeCount;

		frameTime -= invFrameRate;
	}
	
	keyframeFactor = frameTime / invFrameRate;
}

void ModelAnimPlayer::UpdateBone()
{

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		D3DXMATRIX matAnimation;
		D3DXMATRIX matParentAnimation;

		D3DXMatrixIdentity(&matAnimation);

		D3DXMATRIX matInvBindPose = bone->AbsoluteTransform();
		D3DXMatrixInverse(&matInvBindPose, NULL, &matInvBindPose);

		ModelKeyframe* frame = currentClip->Keyframe(bone->Name());
		if (frame == NULL)
			continue;

		if (bUseSlerp == true)
		{
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

			matAnimation = S * R * T;
		}
		else
		{
			matAnimation = frame->Datas[currentKeyframe].Transform;
		}

		int parentIndex = bone->ParentIndex();
		
		if (parentIndex < 0)
		{
			D3DXMATRIX S;
			D3DXMatrixScaling(&S, 0.1f, 0.1f, 0.1f);
			matAnimation *= S;
			D3DXMatrixIdentity(&matParentAnimation);
		}
		else
			matParentAnimation = boneAnimation[parentIndex];


		boneAnimation[i] = matAnimation * matParentAnimation;
		skinTransform[i] = matInvBindPose * boneAnimation[i];
	}
}
