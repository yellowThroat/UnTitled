#pragma once

class ModelAnimPlayer
{
public:
	enum class Mode
	{
		Play = 0, Pause, Stop,
	};

	ModelAnimPlayer(class Model* model);
	~ModelAnimPlayer();

	void Update();
	void Render();

private:
	void UpdateTime();
	void UpdateBone();

private:
	Shader* shader;

	Model* model;
	class ModelAnimClip* currentClip;

	
	Mode mode;

	bool bUseSlerp;

	int currentKeyframe;
	float frameTime; //현재 프레임부터 경과시간
	float keyframeFactor; //현재 프레임과 다음 프레임 사이의 보간 값
	int nextKeyframe;

	vector<D3DXMATRIX> boneAnimation;
	vector<D3DXMATRIX> skinTransform;

	D3DXMATRIX mat;
	D3DXVECTOR3 r;

};