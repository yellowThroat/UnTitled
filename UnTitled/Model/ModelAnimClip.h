#pragma once
#include "../Interfaces/ICloneable.h"

struct ModelKeyframeData
{
	D3DXMATRIX Transform;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale;
};

struct ModelKeyframe : public ICloneable
{
	wstring BoneName;
	vector<ModelKeyframeData> Datas;

	void Clone(void** clone)
	{
		ModelKeyframe* frame = new ModelKeyframe();

		frame->BoneName = BoneName;
		for (ModelKeyframeData temp : Datas)
		{
			ModelKeyframeData data;
			data.Rotation = temp.Rotation;
			data.Transform = temp.Transform;
			data.Translation = temp.Translation;
			data.Scale = temp.Scale;

			frame->Datas.push_back(data);
		}

		*clone = frame;
	}
};

class ModelAnimClip : public ICloneable
{
public:
	friend class Model;
	friend class Models;

public:
	void Clone(void** clone);

	wstring Name() { return name; }

	int TotalFrame() { return totalFrame; }
	float FrameRate() { return frameRate; }

	ModelKeyframe* Keyframe(wstring name)
	{
		return keyframeMap[name];
	}

private:
	ModelAnimClip();
	~ModelAnimClip();

	wstring name;

	int totalFrame;
	float frameRate;
	float defaltFrameRate;

	unordered_map<wstring, ModelKeyframe *> keyframeMap;
};