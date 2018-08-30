#pragma once
#include "WeatherType.h"

class Snow
{
public:
	Snow(WeatherCube& cube, UINT count, ExecuteValues* val = NULL);
	~Snow();

	void Render();
	void PostRender();
	void Velocity(D3DXVECTOR3& vec);
	void Values(ExecuteValues* val) { values = val; }
private:
	Shader* shader;
	vector<struct VertexSnow> vertices;
	vector<UINT> indices;

	UINT particleCount;
	Texture* SnowMap;

	ID3D11BlendState* alphaBlend[2];

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ExecuteValues* values;

	WorldBuffer* world;
private:

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.DrawDistance = 1000;
			Data.Velocity = D3DXVECTOR3(-1, -10, -5);
			Data.Turbulence = 10.0f;
		}

		struct Struct
		{
			D3DXVECTOR3 Velocity;		// ������ �ӵ�
			float		DrawDistance;	// ������ �Ÿ�

			D3DXCOLOR	Color;			// �÷�

			D3DXVECTOR3 Origin;			// ����(min)
			float		Padding0;
			D3DXVECTOR3 Size;
			float		Turbulence;
		}Data;
	};

	Buffer* buffer;
};

