#pragma once

class BillBoard
{
public:
	BillBoard(D3DXVECTOR3 position, int instanceCount, float radius);
	~BillBoard();

	void Update();
	void Render();

	void SetTexture(wstring file);
	static void AddTexture(wstring file);
private:
	void Create();

private:
	typedef VertexTexture VertexType;

	struct InstanceType
	{
		D3DXVECTOR3 position;
	};

private:
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * instanceBuffer;
	ID3D11InputLayout* layout;
	Texture* texture;
	D3DXVECTOR3 position;
	int vertexCount;
	int instanceCount;
	float radius;


	static Shader* shader;
	static int reference;

public:
	static unordered_map<wstring, Texture*> textures;
};