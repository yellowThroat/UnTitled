#pragma once

struct Binder
{
	Binder()
		: currentKeyframe(0), frameTime(0.0f), keyframeFactor(0.0)
		, nextKeyframe(0), elapsedTime(0.0f), bLoop(true), cut(1)
	{
	}

	Binder& operator=(class ModelAnimClip* clip)
	{
		this->clip = clip;

		return *this;
	}

	void Initialize()
	{
		clip = NULL;
		currentKeyframe = 0;
		frameTime = 0.0f;
		keyframeFactor = 0.0f;
		nextKeyframe = 0;
		elapsedTime = 0.0f;
		cut = 1;
		bLoop = true;
	}

	void UpdateTime();

	D3DXMATRIX GetCurrentTransform(wstring name,
		D3DXVECTOR3& vS, D3DXQUATERNION& vQ, D3DXVECTOR3& vT);
	void CopyBinder(Binder* binder);
	bool IsDone();
	bool Combo();

	class ModelAnimClip* clip;

	int currentKeyframe;
	int nextKeyframe;
	int cut;				//한계 프레임 짜르기
	float elapsedTime;		//프레임 경과 시간
	float frameTime;		//현재 프레임부터 경과시간
	float keyframeFactor;	//현재 프레임과 다음 프레임 사이의 보간 값
	bool bLoop;
	UINT index;
};

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
public:
	void World(D3DXMATRIX* world);
	void RootAxis(D3DXMATRIX root);
	D3DXMATRIX GetTransform(UINT index);
	Binder* CurrentClip() { return currentClip; }
	Binder* NextClip() { return nextClip; }
	void Play(UINT index, float blendTime, bool bLoop, int cut = 0);

private:
	void UpdateTime();
	void UpdateBone();
	void Stop();
	void Play();
	void Pause();

private:
	Shader* shader;

	Model* model;
	Binder* currentClip;
	Binder* nextClip;

	Mode mode;

	string clipList;

	UINT BindCount;

	float blendTime;

	vector<D3DXMATRIX> boneAnimation;
	vector<D3DXMATRIX> skinTransform;

	D3DXMATRIX* world;
	D3DXMATRIX rootAxis;
};