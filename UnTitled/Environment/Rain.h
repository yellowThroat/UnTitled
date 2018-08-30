#pragma once
#include "WeatherType.h"

class Rain
{
public:
	Rain(struct WeatherCube& cube, UINT count, ExecuteValues* val = NULL);
	~Rain();

	void Render();
	void PostRender();
private:
	Shader* shader;
	vector<struct VertexRain> vertices;
	vector<UINT> indices;

	UINT particleCount;
	Texture* texture;

	ID3D11BlendState* alphaBlend[2];

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	WorldBuffer* world;
	ExecuteValues* values;
	bool bView;
private:

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.DrawDistance = 1000;
			Data.Velocity = D3DXVECTOR3(-1, -200, -5);
			Data.Definition = 0.2f;
		}

		struct Struct
		{
			D3DXVECTOR3 Velocity;		// 움직일 속도
			float		DrawDistance;	// 보여질 거리

			D3DXCOLOR	Color;			// 컬러

			D3DXVECTOR3 Origin;			// 원점(min)
			float		Padding0;
			D3DXVECTOR3 Size;
			float		Definition;
		}Data;
	};

	Buffer* buffer;
};

