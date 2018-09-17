#pragma once

struct VertexTrail
{
	D3DXVECTOR2 uv;
	UINT order;
};

class WeaponTrail
{
public:
	WeaponTrail(UINT size = 10, UINT length = 10);
	~WeaponTrail();

	void Update();
	void Render();

	void CreateVertex();
	void CreateBuffer();
	void Resize(UINT size, UINT length);

	void SetData(D3DXVECTOR3 bottom, D3DXVECTOR3 top);

private:
	Shader* shader;
	Texture* texture;
	ID3D11Buffer* buffer;
	ID3D11Buffer* indexBuffer;
	VertexTrail* vertices;
	UINT* indices;
	UINT indexCount, vertexCount;
	ID3D11BlendState* alphaBlend[2];
	ID3D11RasterizerState* rasterizer[2];
	UINT size;
	float interval;
	float elapsed;
private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer(UINT size = 10, UINT length = 10) : ShaderBuffer(&Data, sizeof(Struct))
		{
			for (UINT i = 0; i < 100; i++)
			{
				Data.top[i] = D3DXVECTOR4(0, 0, 0, 1);
				Data.Bottom[i] = D3DXVECTOR4(0, 0, 0, 1);
			}
			Data.start = 1;
			Data.size = size;
			Data.length = length;
		}

		struct Struct
		{
			D3DXVECTOR4 top[100];
			D3DXVECTOR4 Bottom[100];
			int start;
			UINT size;
			UINT length;
			float Padding;
		}Data;
	};

	Buffer* vsBuffer;
};