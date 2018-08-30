#pragma once
#include "../Objects/GameModel.h"

class SkyPlane
{
private:
	struct SkyPlaneAttribute
	{
		int skyPlaneResolution;
		int textureRepeat;
		float skyPlaneWidth;
		float skyPlaneTop;
		float skyPlanebottom;

		SkyPlaneAttribute()
		{
			skyPlaneResolution = 10;
			textureRepeat = 4;
			skyPlaneWidth = 10.0f;
			skyPlaneTop = 0.5f;
			skyPlanebottom = 0.0f;
		}
	};
	struct SkyPlaneType
	{
		float x, y, z;
		float tu, tv;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 uv;
	};

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.translation = 0.0f;
			Data.brightness = 0.0f;
			Data.scale = 0.0f;
		}

		struct Struct
		{
			float translation;
			float scale;
			float brightness;
			float padding;
		}Data;
	};

public:
	SkyPlane();
	~SkyPlane();

	bool Initialize(wstring, wstring);
	void Shutdown();
	void Render();
	void Frame();
	void PostRender();

	int GetIndexCount();

	ID3D11ShaderResourceView* GetCloudTexture1();
	ID3D11ShaderResourceView* GetCloudTExture2();

	float GetBrightness();
	void SetWorld(D3DXVECTOR3 pos);

private:
	bool InitializeSkyPlane(int, float, float, float, int);
	bool InitializeSkyPlane(SkyPlaneAttribute att);
	void ShutdownSkyPlane();

	bool InitializeBuffers(int);
	void CreateData(int skyPlaneResolution);
	void CreateBuffer();
	void ShutdownBuffers();
	void RenderBuffers();

	bool LoadTextures(wstring, wstring);
	void ReleaseTextures();

private:
	SkyPlaneType* skyPlane;
	SkyPlaneAttribute att;
	int vertexCount;
	int indexCount;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	VertexType* vertices;
	DWORD* indices;

	Texture* cloudTexture1;
	Texture* cloudTexture2;
	Texture* cloudTexture3;
	float translationSpeed;
	Buffer* buffer;
	Shader* shader;
	WorldBuffer* world;
	bool view;
	
};
