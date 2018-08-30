#pragma once
#include "../Objects/GameRender.h"

class ProjectionTexture : public GameRender
{
public:
	ProjectionTexture();
	~ProjectionTexture();

	void Update() override;
	void Render() override;

	void Save(BinaryWriter* w) override;
	void Load(BinaryReader* r) override;

	void PostRender() override;
	
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Sturct))
		{
			D3DXMatrixIdentity(&Data.Projection);
			D3DXMatrixIdentity(&Data.View);
		}

		void SetView(D3DXMATRIX mat)
		{
			Data.View = mat;
			D3DXMatrixTranspose(&Data.View, &Data.View);
		}

		void SetProjection(D3DXMATRIX mat)
		{
			Data.Projection = mat;
			D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
		}

		struct Sturct
		{
			D3DXMATRIX View;
			D3DXMATRIX Projection;
		}Data;
	};

	class FixedCamera* camera;
	class Perspective* perspective;
	class Texture* texture;
	Buffer* buffer;
};
