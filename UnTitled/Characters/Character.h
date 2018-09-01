#pragma once
#include "../Objects/GameRender.h"

class Character : public GameRender
{
public:
	Character();
	~Character();

	virtual void Update();
	virtual void Render();

	D3DXMATRIX GetBoneTransform(UINT index);
	D3DXMATRIX GetBoneTransform(wstring name);

protected:
	Model* model;
	class ModelAnimPlayer* anim;
};