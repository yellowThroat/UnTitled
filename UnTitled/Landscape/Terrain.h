#pragma once
#include "../Objects/GameRender.h"

class BinaryReader;
class BinaryWriter;

class Ocean;
class BillBoard;

namespace Landscape
{
	class GridBuffer : public ShaderBuffer
	{
	public:

		GridBuffer()
			: ShaderBuffer(&Grid, sizeof(Grid))
		{
			Grid.Color = D3DXCOLOR(0, 1, 0, 1);
			Grid.Spacing = 1;
			Grid.Thickness = 0.03f;
			Grid.bView = 0;
		}

		struct Struct
		{
			D3DXCOLOR Color;
			UINT Spacing;
			float Thickness;
			int bView;

			float Padding;

		}Grid;
	};

	class BrushBuffer : public ShaderBuffer
	{
	public:

		BrushBuffer()
			: ShaderBuffer(&Info, sizeof(Info))
		{
			Info.Color = D3DXCOLOR(1, 0, 0, 1);
			Info.Position = D3DXVECTOR3(-5, -5, -5);
			Info.Type = 0;
			Info.Range = 1;
			Info.Before = D3DXVECTOR3(-5, -5, -5);
			Info.Select = 0;
			Info.Speed = 0.5f;
		}

		struct Struct
		{
			D3DXCOLOR Color;
			D3DXVECTOR3 Position;
			UINT Type;
			float Range;
			D3DXVECTOR3 Before;
			int Select;
			float Speed;
			float Padding[2];
		}Info;
	};

	class Terrain : public GameRender
	{
	public:
		Terrain(ExecuteValues* values);
		~Terrain();

		float GetVertexHeight(D3DXVECTOR3 position);

		void PreRender() override;
		void Update() override;
		void Render() override;
		void PostRender() override;

	public:
		Material* GetMaterial() { return material; }
		void SetDiffuse(wstring file = L"");
		void SetTexture(wstring file = L"", UINT index = 0);
		void SetHeight(wstring file = L"");
		class Data* GetData() { return data; }


		void ShaderFile(Shader* s = NULL) override;
		void InitSettings() override;

		void Save(BinaryWriter* w) override;
		void Load(BinaryReader* r) override;
	private:
		void InputHandle();

	private:
		ExecuteValues* values;
		class Data* data;
		Ocean* ocean;

		Material* material;

		Texture* texture[4];
		
		WorldBuffer* worldBuffer;
		BrushBuffer* brushBuffer;
		GridBuffer* gridBuffer;
		
		POINT size;
		float flatteningHeight;
		D3DXVECTOR4 textureBlend;
		int bTextureBlend;
		int mode;
		int width;
		int height;
		int billBoardCount;
		wstring currentBillBoard;
		Shader* billBoardShader;
		Shader* billBoardShadowShader;
		vector<BillBoard*> billBoards;
		
	public:
		static Texture* brush;
	};
}