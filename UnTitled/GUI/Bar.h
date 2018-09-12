#pragma once

class Bar
{
public:
	struct VertexBar
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 uv;
		UINT isBottom;
	};

public:
	Bar(wstring bottom, wstring top, D3DXVECTOR3* target);
	~Bar();

	void Update();
	void Render();
	void SetTarget(D3DXVECTOR3* target);
	void SetRatio(float val);

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer()
			: ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Position = D3DXVECTOR3(0, 0, 0);
			Data.Length = 3.0f;
			Data.Width = 0.3f;
			Data.Height = 7.0f;
			Data.Ratio = 1.0f;
		}

		struct Struct
		{
			D3DXVECTOR3 Position;
			float Padding;
			float Height;
			float Width;
			float Length;
			float Ratio;
			
		}Data;
	};

private:
	void CreateVertex();
	void CreateBuffer();
	void SetTexture(wstring bottom, wstring top);
	
	Shader* shader;
	WorldBuffer* world;
	VertexBar* data;
	UINT* indexData;
	ID3D11Buffer* buffer;
	ID3D11Buffer* indexBuffer;
	UINT vertexCount;
	UINT indexCount;
	Buffer* vsBuffer;
	Texture* bottom;
	Texture* top;
	D3DXVECTOR3* target;
	ID3D11BlendState* alphaBlend[2];
	float maxRatio;
};