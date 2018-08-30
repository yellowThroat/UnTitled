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
	float frameTime; //���� �����Ӻ��� ����ð�
	float keyframeFactor; //���� �����Ӱ� ���� ������ ������ ���� ��
	int nextKeyframe;

	vector<D3DXMATRIX> boneAnimation;
	vector<D3DXMATRIX> skinTransform;

	D3DXMATRIX mat;
	D3DXVECTOR3 r;

};