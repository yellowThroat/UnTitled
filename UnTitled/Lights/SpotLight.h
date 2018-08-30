#pragma once
#include "../Objects/GameRender.h"

namespace Shapes
{
	class Cone;
}
class SpotLight : public GameRender
{
public:
	SpotLight();
	~SpotLight();

	void Update() override;
	void Render() override;

	void Save(BinaryWriter* w) override;
	void Load(BinaryReader* r) override;

	void PostRender() override;

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 32; i++)
			{
				Data.Light[i].Position = D3DXVECTOR3(0, 0, 0);
				Data.Light[i].Color = D3DXVECTOR3(1, 1, 1);
				Data.Light[i].Direction = D3DXVECTOR3(0, 0, 0);
				Data.Light[i].InnerAngle = 10.0f;
				Data.Light[i].OuterAngle = 30.0f;
				Data.Light[i].Range = 5;
			}

			Data.Count = 0;
		}

		struct Light
		{
			D3DXVECTOR3	Position;
			float		InnerAngle;
			D3DXVECTOR3 Color;
			float		OuterAngle;
			D3DXVECTOR3 Direction;
			float		Range;
		};

		struct Struct
		{
			Light Light[32];

			int Count;
			float Padding[3];
		}Data;
	};

	static Buffer* buffer;
	Shapes::Cone* cone;
	int index;
};
