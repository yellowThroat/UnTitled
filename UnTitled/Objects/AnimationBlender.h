#pragma once
#include "AnimationBinder.h"

struct AnimationBlender
{
	AnimationBinder* Current = NULL;
	AnimationBinder* Next = NULL;

	wstring Name = L"";

	UINT BindCount = 0;
	float BlendTime = 0.0f;
	float ElapsedTime = 0.0f;

	bool Empty() { return BindCount == 0; }
	bool Exist() { return Empty() == false && Current != NULL; }
	AnimationBlender()
	{
		Current = new AnimationBinder();
		Next = new AnimationBinder();

		BindCount = 0;
	}

	void ResetBinder()
	{
		Current->Initialize();
		Next->Initialize();

		BindCount = 0;
	}

	void ShiftBinder()
	{
		Current->Initialize();
		Next->CopyTo(Current);
		Next->Initialize();

		BindCount--;
	}

	void AddKeyframe(AnimationKeyframe* keyframe, float startTime, float blendTime, float timeScaleFactor, AnimationPlayMode mode)
	{
		BlendTime = blendTime;
		ElapsedTime = startTime;

		if (BlendTime == 0.0f)
		{
			ResetBinder();

			Current->BindKeyframe(keyframe, startTime, timeScaleFactor, mode);
		}
		else
		{
			if (BindCount == 2)
				ShiftBinder();

			if (BindCount == 0)
				Current->BindKeyframe(keyframe, startTime, timeScaleFactor, mode);
			else if (BindCount == 1)
				Next->BindKeyframe(keyframe, startTime, timeScaleFactor, mode);
		}

		BindCount++;
	}

	D3DXMATRIX Negative()
	{
		D3DXMATRIX scaleX;
		D3DXMatrixScaling(&scaleX, -1, 1, 1);

		D3DXMATRIX rotationX;
		D3DXMatrixRotationX(&rotationX, -Math::ToRadian(90));

		D3DXMATRIX rotationY;
		D3DXMatrixRotationY(&rotationY, Math::ToRadian(180));

		return scaleX * rotationX * rotationY;
	}

	D3DXMATRIX GetKeyframeMatrix(float time)
	{
		D3DXMATRIX matrix;

		ElapsedTime += time;
		if (BindCount <= 1)
		{
			matrix = Current->GetKeyframeMatrix(time);
		}
		else
		{
			float t = ElapsedTime / BlendTime;
			if (t > 1.0f)
			{
				matrix = Next->GetKeyframeMatrix(time);

				ShiftBinder();
			}
			else
			{
				AnimationKeyframeData target;

				AnimationKeyframeData src[2];
				src[0] = Current->GetKeyframeData(time);
				src[1] = Next->GetKeyframeData(time);

				CalcBlendingKeyframe(&target, src, t);

				matrix = CreateTransformFromTarget(target);
			}
		}//if(BindCount)

		return Negative() * matrix * Negative();
	}

	void CalcBlendingKeyframe(AnimationKeyframeData* target, AnimationKeyframeData* src, float t)
	{
		//Translation
		if (src[0].bHasTranslation == true && src[1].bHasTranslation == true)
		{
			D3DXVec3Lerp(&target->Translation, &src[0].Translation, &src[1].Translation, t);
			target->bHasTranslation = true;
		}
		else if (src[0].bHasTranslation == true)
		{
			target->Translation = src[0].Translation;
			target->bHasTranslation = true;
		}
		else if (src[1].bHasTranslation == true)
		{
			target->Translation = src[1].Translation;
			target->bHasTranslation = true;
		}


		//Scale
		if (src[0].bHasScale == true && src[1].bHasScale == true)
		{
			D3DXVec3Lerp(&target->Scale, &src[0].Scale, &src[1].Scale, t);
			target->bHasScale = true;
		}
		else if (src[0].bHasScale == true)
		{
			target->Scale = src[0].Scale;
			target->bHasScale = true;
		}
		else if (src[1].bHasScale == true)
		{
			target->Scale = src[1].Scale;
			target->bHasScale = true;
		}


		//Rotation
		if (src[0].bHasRotation == true && src[1].bHasRotation == true)
		{
			D3DXQuaternionSlerp(&target->Rotation, &src[0].Rotation, &src[1].Rotation, t);
			target->bHasRotation = true;
		}
		else if (src[0].bHasRotation == true)
		{
			target->Rotation = src[0].Rotation;
			target->bHasRotation = true;
		}
		else if (src[1].bHasRotation == true)
		{
			target->Rotation = src[1].Rotation;
			target->bHasRotation = true;
		}
	}

	D3DXMATRIX CreateTransformFromTarget(AnimationKeyframeData target)
	{
		D3DXMATRIX matrix;

		if (target.bHasRotation == true)
			D3DXMatrixRotationQuaternion(&matrix, &target.Rotation);

		if (target.bHasTranslation == true)
		{
			matrix._41 = target.Translation.x;
			matrix._42 = target.Translation.y;
			matrix._43 = target.Translation.z;
		}

		if (target.bHasScale == true)
		{
			D3DXMATRIX temp;
			D3DXMatrixScaling(&temp, target.Scale.x, target.Scale.y, target.Scale.z);

			matrix = matrix * temp;
		}

		return matrix;
	}
};