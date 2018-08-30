#pragma once

class BinaryReader;
class BinaryWirther;

namespace Landscape
{
	class GridBuffer;
	class BrushBuffer;

	class Data
	{
	private:
		struct Quad
		{
			D3DXVECTOR3 v1;
			D3DXVECTOR3 v2;
			D3DXVECTOR3 v3;
			D3DXVECTOR3 v4;

			Quad* quad[4];

			Quad()
			{
				v1 = D3DXVECTOR3(0, 0, 0);
				v2 = D3DXVECTOR3(0, 0, 0);
				v3 = D3DXVECTOR3(0, 0, 0);
				v4 = D3DXVECTOR3(0, 0, 0);

				for (int i = 0; i < 4; i++)
				{
					quad[i] = NULL;
				}
			}
		};

	public:
		typedef VertexColorTextureNormal VertexType;

	public:
		Data(wstring file = L"", float heightRatio = 10.0f);
		~Data();

		float GetHeight(D3DXVECTOR3 position);

		void SetPlane(int x = 256, int y = 256);
		void SetHeightMapFile(wstring file, float heightRatio = 10.0f);
		void UpdateVertexData(D3D11_BOX* box = NULL);

		void Render();

	public:
		void SetColor(D3DXCOLOR color, int x, int y);
		void SetColor(D3DXCOLOR color);
		void SetHeight(GridBuffer& grid, BrushBuffer& brush, bool upHeight);
		void Smooth(GridBuffer& grid, BrushBuffer & brush);
		void Flattening(GridBuffer& grid, BrushBuffer& brush, float height);
		void CopyData(VertexType** data, VertexType** data1, UINT** index);
		void SetData(VertexType * data, VertexType* data1, UINT * index, UINT vCount, UINT iCount);

		bool GetPosition(ExecuteValues* values, D3DXMATRIX world, OUT D3DXVECTOR3* location);
		UINT GetWidth() { return width; }
		UINT GetHeight() { return height; }
		UINT GetVertexCount() { return vertexCount; }
		UINT GetIndexCount() { return indexCount; }
		VertexType* GetVertexData() { return vertexData; }
		VertexType* GetVertices() { return vertices; }
		UINT* GetIndexData() { return indexData; }

		void SaveData(BinaryWriter* w);
		void LoadData(BinaryReader* r);
		void FillNormalData();
	private:
		void InitQuad(Quad* quad, float x, float z, float width, float height);
		BOOL GetQuad(Quad* quad, D3DXVECTOR3 start, D3DXVECTOR3 direction, D3DXVECTOR4& q);
		float GetAverage(UINT x, UINT y);
		void Clear();

		void FillVertexData(int x, int y, float heightRatio);
		
		void CreateBuffer();
		void VertexBind();
	private:
		Texture* heightMap;

		UINT width, height;
		UINT vertexCount, indexCount;
		ID3D11Buffer* vertexBuffer, *indexBuffer;

		VertexType* vertexData;
		VertexType* vertices;
		UINT* indexData;
		Quad* root;
	};
}