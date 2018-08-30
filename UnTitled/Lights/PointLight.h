#pragma once
#include "../Objects/GameRender.h"

namespace Shapes
{
	class Sphere;
}

class PointLight : public GameRender
{
public:
	PointLight();
	~PointLight();

	void Update() override;
	void Render() override;

	void Save(BinaryWriter* w) override;
	void Load(BinaryReader* r) override;

	void PostRender() override;

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer()
			: ShaderBuffer(&Data, sizeof(Struct))
		{
			for (int i = 0; i < 32; i++)
			{
				Data.Light[i].Position = D3DXVECTOR3(0, 0, 0);
				Data.Light[i].Color = D3DXVECTOR3(1, 1, 1);
				Data.Light[i].intensity = 1;
				Data.Light[i].Range = 5;
			}

			Data.Count = 0;
		}
		struct Light
		{
			D3DXVECTOR3	Position;
			float		intensity;

			D3DXVECTOR3 Color;
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
	Shapes::Sphere* sphere;
	int index;
};