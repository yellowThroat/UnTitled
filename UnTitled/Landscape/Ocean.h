#pragma once

class Ocean
{
public:
	Ocean
	(
		ExecuteValues* values, UINT width, UINT height, float thick	
	);
	~Ocean();

	void Update();
	void Render();
	void PostRender();
private:
	ExecuteValues* values;

	Material* material;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
	vector<OceanVertexType> vertices;
	vector<UINT> indices;

	UINT width;
	UINT height;
	float thick;

	bool bWireframe;
	ID3D11RasterizerState* rasterizer[2];
	ID3D11BlendState* blend[2];

	Texture* heightMap;
	bool bView;

private:
	class VsBuffer : public ShaderBuffer
	{
	public:
		VsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.WaveFrequency = 0.05f;
			Data.WaveAmplitude = 1.0f;

			Data.TextureScale = D3DXVECTOR2(14, 14);
			Data.BumpSpeed = D3DXVECTOR2(0.0f, 0.05f);
			Data.BumpHeight = 0.6f;
		}

		struct Struct
		{
			float WaveFrequency;
			float WaveAmplitude;

			D3DXVECTOR2 TextureScale;

			D3DXVECTOR2 BumpSpeed;
			float BumpHeight;
			
			float Padding;
		} Data;
	};

	class PsBuffer : public ShaderBuffer
	{
	public:
		PsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.ShallowColor = D3DXCOLOR(0.7f, 0.85f, 0.8f, 1.0f);
			Data.DeepColor = D3DXCOLOR(0.2f, 0.5f, 0.95f, 1.0f);

			Data.FresnelBias = 0.8f;
			Data.FresnelPower = 0.5f;
			Data.FresnelAmount = 0.5f;
			Data.SpecularAmount = 3.0f;

			Data.HeightRatio = 7.5f;
			Data.ShoreBlend = 35.0f;
		}

		struct Struct
		{
			D3DXCOLOR ShallowColor; //������
			D3DXCOLOR DeepColor; //������

			float FresnelBias; //����
			float FresnelPower; //����
			float FresnelAmount; //������

			float SpecularAmount; //�ݻ簭��

			D3DXVECTOR2 OceanSize; //���ؽ��� ��üũ��
			float HeightRatio; //���� ������
			float ShoreBlend; //���İ� ��ȭ����
		} Data;
	};

	VsBuffer* vsBuffer;
	PsBuffer* psBuffer;
};