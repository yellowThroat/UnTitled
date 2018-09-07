#pragma once

class Grid
{
public:
	Grid();
	~Grid();

	void Render();

private:
	void CreateData();
	void CreateBuffer();

private:
	VertexColor* _data;
	UINT* _index;
	ID3D11Buffer* _buffer;
	ID3D11Buffer* _indexBuffer;
	WorldBuffer* _world;
	Shader* _shader;

	UINT _vertexCount;
	UINT _indexCount;
	ID3D11DepthStencilState* depthStencilState[2];
	ID3D11BlendState* alphaBlend[2];

};