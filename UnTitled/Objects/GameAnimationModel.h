#pragma once
#include "GameModel.h"
#include "AnimationClip.h"

class GameAnimationModel : public GameModel
{
public:
	GameAnimationModel(wstring meshFile, wstring matFile);
	virtual ~GameAnimationModel();

	UINT AddClip(wstring file);
	UINT AddClip(struct AnimationClip* clip);
	void RemoveClip(UINT index);
	void ClearClips();

	struct AnimationClip* GetClip(UINT index);
	struct AnimationBlender* GetBlenderFromBoneName(wstring name);

	bool Play(UINT index, enum class AnimationPlayMode mode);
	bool Play(UINT index, float startTime, float blendTime, float timeScaleFactor, enum class AnimationPlayMode mode);

	float GetPlayTime(wstring boneName);

	virtual void Update();
	virtual void Render();

private:
	vector<struct AnimationBlender *> blenders;
	vector<struct AnimationClip *> clips;
};