#pragma once
#include "../Objects/GameRender.h"

class Sky : public GameRender
{
public:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Apex = D3DXCOLOR(0xFF9BCDFF);
			Data.Center = D3DXCOLOR(0xFF0080FF);
			Data.Height = 1.0f;
		}

		struct Struct
		{
			D3DXCOLOR Center;
			D3DXCOLOR Apex;
			float Height;
			float Padding[3];
		}Data;
	};

	Sky(ExecuteValues* values);
	~Sky();

	void Update();
	void Render();
	void PostRender();

private:
	Model* model;
	ExecuteValues* values;

	Buffer* buffer;

	ID3D11RasterizerState* rasterizer[2];
	ID3D11DepthStencilState* depthStencilState[2];
	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11BlendState* alphaDisableBlendingState;
	ID3D11BlendState* alphaBlendState2;

	class SkyPlane* plane;
private:
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();
	void EnableSecondBlendState();
	bool view;
};
